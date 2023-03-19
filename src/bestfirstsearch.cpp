/*
 *  depthfirstsearch.cpp
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "bestfirstsearch.h"
#include "util.h"
#include <time.h>
#include <algorithm>
#include "globals.h"
#include "colors.h"
#include <functional>
#include <queue>

using namespace std;

BestFirstSearch::BestFirstSearch(Node & root,
        double maxTime,
        int maxNodes,
        int maxSolutions,
        string solFileName,
        string summaryFileName,
        int verbosity,
        int backjump,
        int dbSize,
        int sampling,
        int threshold,
        bool only_them_words, bool prune_with_bps)
: Search(root, maxTime, maxNodes, maxSolutions, verbosity),
m_solutionSampling(sampling),
m_lastProcessedNode(root), m_nodeThresholdForRestart(threshold) {
    m_solFileName = solFileName.c_str();
    m_summaryFileName = summaryFileName.c_str();
    m_summaryInfo = "";
    m_resourceLimit = false;
    m_nodesSinceLastRestart = 0;
    this->m_maxTSR = 0;
    this->m_bestSolScore = 0;
    pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&count_mutex);
    BestFirstSearch::DONE = 0;
    pthread_mutex_unlock(&count_mutex);
    time_to_report = 0;
}

BestFirstSearch::BestFirstSearch(Node & root, string summaryFileName)
: Search(root, g_pm.getMaxTime(), g_pm.getMaxNodes(), g_pm.getMaxSolutions(), g_pm.getVerbosity()),
m_solutionSampling(g_pm.getSolutionSampling()),
m_lastProcessedNode(root), m_nodeThresholdForRestart(g_pm.getNodeThresholdForRestart())
{
    m_solFileName = g_pm.getSolFileName().c_str();
    m_summaryFileName = summaryFileName.c_str();
    m_summaryInfo = "";
    m_resourceLimit = false;
    m_nodesSinceLastRestart = 0;
    this->m_maxTSR = 0;
    this->m_bestSolScore = 0;
    pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&count_mutex);
    BestFirstSearch::DONE = 0;
    pthread_mutex_unlock(&count_mutex);
    time_to_report = 0;
}

BestFirstSearch::~BestFirstSearch() {
    m_summaryInfo = getSummaryInfo();
    cerr << m_summaryInfo << endl;
    ofstream ofile(g_pm.getScoreFileName().c_str(), ios::out);
    if (ofile) {
        ofile << this->m_bestSolScore << endl;
        ofile.close();
    }
}

int BestFirstSearch::runSearch() {
    m_root.setSkipDeadlockExtraction(true);
    this->m_bestSolScore = .1;
    if (g_pm.getHeurOrder() != 12 && g_pm.getHeurOrder() != 13)
        g_pm.setHeurOrder(12);

    std::priority_queue<Node> q;
    q.push(m_root);

    while (!q.empty()) {
        try {
            Node node = q.top();
            q.pop();
            int time_in_secs = this->getElapsedTime();
            if (time_in_secs >= this->time_to_report) {
                stringstream msg;
                msg << "Timeline " << time_in_secs << " best score " << this->m_bestSolScore 
                    << " open size " << q.size() << endl;
                cerr << msg.str();
                this->time_to_report += 60;
            }
            this->processNode(node);
            if (node.isGoal())
                this->m_nrSolutionsFound++;
            if (this->m_maxNrSolutions >= 0 && this->m_nrSolutionsFound >= this->m_maxNrSolutions)
                break;
            if (resourceLimit()) {
                m_resourceLimit = true;
                break;
            }
            vector<Node> successors = node.generateSuccessors(m_bestSolScore, false, false);
            this->m_expandedNodes++;
            m_bfStatsCol.addDataPoint(node.getDepth(), successors.size());
            m_allVisitedNodes += node.getNrAllSuccessors();
            m_visitedNodes += successors.size();
            time_t current;
            time(&current);
            int nr_succs = successors.size();
            for (int it = 0; it < nr_succs; it++) {
                if (g_pm.getDelayedStaticAnal())
                    if (!successors[it].staticAnalAndTest(m_bestSolScore, g_pm.getPruneWithBPs()))
                        continue;
                q.push(successors[it]);
            }
            //cerr << "Size of Open in KB: " << (1.0*q.size()*sizeof(Node))/(1024) << endl;
        } catch (const std::bad_alloc& e) {
           cerr << "Allocation failed: " << e.what() << '\n';
           m_resourceLimit = true;
           break;
        }
    }

    if (q.empty() && this->m_nrSolutionsFound == 0)
        this->m_nrSolutionsFound = -1;

    if (this->getNrSolsFound() == -1 || this->getNrSolsFound() > 0) {
        if (g_pm.getStopThreadsOnCompletion()) {
            pthread_mutex_t count_mutex  = PTHREAD_MUTEX_INITIALIZER;
            pthread_mutex_lock(&count_mutex);
            //cerr << "Mutex locked" << endl;
            BestFirstSearch::DONE = 1;
            pthread_mutex_unlock(&count_mutex);
        }
    }
    //cerr << "Mutex unlocked" << endl;
    stringstream msg;
    msg << "Full or partial solution with a highest score achieved:" << endl <<
           this->m_bestPartSol << endl;
    cerr << msg.str();
    return 0;
}

void BestFirstSearch::processNode(const Node & node) {
    int nr_them_points = node.getData().getNrThematicPoints();
    //double score = node.getData().getF();
    double score = node.getData().getNrThematicPoints();
    int gridsize = node.getData().getNrRows()*node.getData().getNrCols();
    score += .5*(node.getData().getNrFilledCells())/gridsize;
    //node.getData().getPointsPerCell();
    if (score > this->m_bestSolScore) {
        this->m_bestSolScore = score;
        this->m_bestPartSol = node;
        //cerr << "Score: " << score << endl;
    }
    if ((g_pm.getVerbosity() > 0 && score >= this->m_bestSolScore) ||
    score > this->m_bestSolScore) {
        stringstream msg;
        msg << "\n\033[1;31mNew partial solution with a high score found in BFS after " <<
            getElapsedTime() << " seconds" << reset << endl <<
            "Nr solutions: " << m_nrSolutionsFound <<
            " Exp nodes: " << m_expandedNodes <<
            " Visited nodes: " << m_visitedNodes << endl << node;
        cerr << msg.str();
    }
    if (node.isGoal()) {
        if (g_pm.getVerbosity() > 0)
            cerr << node << endl;
        if ((false && m_solFileName.length() > 0)) {
            ofstream ofile(m_solFileName.c_str(), ios::out | ios::app);
            if (ofile) {
                ofile << "Nodes: " << m_expandedNodes << endl;
                ofile << "Time since beginning (sec): "
                        << getElapsedTime() << endl << node << endl;
                ofile.close();
                char rbsFileName[200];
                sprintf(rbsFileName, "%s-%d.prb", m_solFileName.c_str(), m_nrSolutionsFound);
                ofstream ofile(rbsFileName, ios::out);
                node.writeAsRbs(ofile);
                ofile.close();
            }
        }
        if ((m_solFileName.length() > 0)) {
            ofstream ofile(m_solFileName.c_str(), ios::out | ios::app);
            if (ofile) {
                for (int row = 0; row < node.getData().getNrRows(); row++) {
                    ofile << node.getData().getRow(row) << endl;
                }
                ofile << endl;
                ofile.close();
            }
        }
        if (m_verbosity >= 2) {
            cerr << "Complete solution found with depth-first search ";
            cerr << node;
        } else if (m_verbosity >= 3) {
            //cerr << getElapsedTime() << " seconds" << endl;
            //cerr << " Exp nodes: " << m_expandedNodes;
            //cerr << " Visited nodes: " << m_visitedNodes; << " ";
            cerr << node;
        }
        m_solDepthStats.addDataPoint(node.getDepth());
        double ratio = node.getData().getPointsPerCell();
        if (ratio > this->m_maxTSR) {
            this->m_maxTSR = ratio;
        }
    }
    if (m_verbosity >= 2 ||
            (m_expandedNodes % 10000 == 0 && m_verbosity >= 1)) {
        //if (m_maxNrSolutions < 0 || m_nrSolutionsFound < m_maxNrSolutions) {
        cerr << "Expanding node in BFS:" << endl;
        cerr << node;
        cerr << "Nodes: " << m_expandedNodes << endl;
        cerr << "Time since beginning (sec): "
                << getElapsedTime() << endl;
        cerr << "============================================="
                << endl << endl;
        //}
    }
}

bool BestFirstSearch::resourceLimit() const {
    if (m_maxNrNodes >= 0 && m_expandedNodes > m_maxNrNodes)
        return true;
    if (m_maxTime >= 0 && getElapsedTime() >= m_maxTime)
        return true;
    return false;
}

bool BestFirstSearch::foundAllSolutions() const {
    if (m_maxNrSolutions >= 0 && m_nrSolutionsFound >= m_maxNrSolutions)
        return true;
    return false;
}

int BestFirstSearch::DONE;
