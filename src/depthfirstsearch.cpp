/*
 *  depthfirstsearch.cpp
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "depthfirstsearch.h"
#include "util.h"
#include <time.h>
#include <algorithm>
#include "globals.h"
#include "colors.h"

using namespace std;

DepthFirstSearch::DepthFirstSearch(Node & root,
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
m_deadlocks(dbSize), m_solutionSampling(sampling),
m_lastProcessedNode(root), m_nodeThresholdForRestart(threshold) {
    m_solFileName = solFileName.c_str();
    m_summaryFileName = summaryFileName.c_str();
    m_summaryInfo = "";
    m_backjump = backjump;
    m_resourceLimit = false;
    m_nodesSinceLastRestart = 0;
    this->m_maxTSR = 0;
    this->m_bestSolScore = 0;
    this->m_onlyThemWords = only_them_words;
    this->m_pruneWithBPs = prune_with_bps;
    this->m_deadlocks.clear();
    pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&count_mutex);
    DepthFirstSearch::DONE = 0;
    DepthFirstSearch::m_discrepBudget = 100000;
    DepthFirstSearch::m_sterileBudget = 105;
    pthread_mutex_unlock(&count_mutex);
}

DepthFirstSearch::DepthFirstSearch(Node & root, string summaryFileName)
: Search(root, g_pm.getMaxTime(), g_pm.getMaxNodes(), g_pm.getMaxSolutions(), g_pm.getVerbosity()),
m_deadlocks(g_pm.getDBSize()), m_solutionSampling(g_pm.getSolutionSampling()),
m_lastProcessedNode(root), m_nodeThresholdForRestart(g_pm.getNodeThresholdForRestart())
{
    m_solFileName = g_pm.getSolFileName().c_str();
    m_summaryFileName = summaryFileName.c_str();
    m_summaryInfo = "";
    m_backjump = g_pm.getBackjump();
    m_resourceLimit = false;
    m_nodesSinceLastRestart = 0;
    this->m_maxTSR = 0;
    this->m_bestSolScore = 0;
    this->m_onlyThemWords = false;
    this->m_pruneWithBPs = g_pm.getPruneWithBPs();
    this->m_deadlocks.clear();
    pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&count_mutex);
    DepthFirstSearch::DONE = 0;
    DepthFirstSearch::m_discrepBudget = 100000;
    DepthFirstSearch::m_sterileBudget = 105;
    pthread_mutex_unlock(&count_mutex);
}

DepthFirstSearch::~DepthFirstSearch() {
    m_summaryInfo = getSummaryInfo();
    cerr << m_summaryInfo << endl;
    ofstream ofile(g_pm.getScoreFileName().c_str(), ios::out);
    if (ofile) {
        ofile << this->m_bestSolScore << endl;
        ofile.close();
    }
}

int DepthFirstSearch::runSearch() {
    DepthFirstSearch::m_nrDevs = 0;
    m_root.setSkipDeadlockExtraction(true);
    m_bestSolScore = g_pm.getMinNrThemPoints();
    if (m_bestSolScore == 0) {
        int nrc = m_root.getData().getNrCols();
        int nrr = m_root.getData().getNrRows();
        m_bestSolScore = nrc * nrr - m_root.getData().getNrBPs();
    }
    this->m_bestSolScore = 5;
    int result = depthFirst(m_root);
    //cerr << "Search completed" << endl;
    //if (m_resourceLimit || this->m_nrSolutionsFound == this->m_maxNrSolutions)
    //    cout << "Exiting before exploring entire space" << endl;
    if (this->getNrSolsFound() == -1 || this->getNrSolsFound() > 0) {
        if (g_pm.getStopThreadsOnCompletion()) {
            pthread_mutex_t count_mutex  = PTHREAD_MUTEX_INITIALIZER;
            pthread_mutex_lock(&count_mutex);
            //cerr << "Mutex locked" << endl;
            DepthFirstSearch::DONE = 1;
            pthread_mutex_unlock(&count_mutex);
        }
    }
    //cerr << "Mutex unlocked" << endl;
    stringstream msg;
    msg << "Full or partial solution with a highest score achieved:" << endl <<
           this->m_bestPartSol << endl;
    cerr << msg.str();
    return result;
}

int DepthFirstSearch::depthFirst(Node & node) {
    time_t start;
    time(&start);
    processNode(node); // e.g., check if is solution, etc.
    //if (node.getDepth() > 10)
    //    return REACHED_MAX_DEPTH;
    m_lastProcessedNode = node;
    m_expandedNodes++;
    m_depthStats.addDataPoint(node.getDepth());
    if ((DepthFirstSearch::DONE == 1) || (resourceLimit())) {
        m_resourceLimit = true;
        return RESOURCE_LIMIT; // stop no matter what
    }
    if (node.isGoal()) {
        this->m_nrSolutionsFound++;
        //cerr << "Nr SOLS: " << this->m_nrSolutionsFound << "  ---- " << this->m_maxNrSolutions << endl;
    }
    if (this->m_maxNrSolutions >= 0 && this->m_nrSolutionsFound >= this->m_maxNrSolutions) {
        return SUCCESS;
    }
    if (g_pm.getDBSize() > 0) {
        if (m_deadlocks.nodeMatchesRecord(node)) {
            if (g_pm.getVerbosity() >= 3) {
                cerr << "A deadlock from the database matched the following node:" << endl;
                cerr << node.getData() << endl;
            }
            node.setRemoteDPInfo(m_deadlocks.getMostRecentHit().getCells());
            return MATCH_DEADLOCK_DB;
        }
    }
    /*
        if (m_nodesSinceLastRestart >= m_nodeThresholdForRestart) {
            if (node.getDepth() > 0)
                return DONTCARE;
            else
                m_nodesSinceLastRestart = 0; 
        }
     */
#ifndef SOLVE_PORTIONS
    if (false && node.getDepth() >= 10) {
        double ratio = 1.0 * node.getData().getNrThematicPoints() / node.getData().getNrFilledCells();
        if (ratio > this->m_maxTSR) {
            this->m_maxTSR = ratio;
            cerr << "New improvement to the best thematic ratio:" << endl;
            cerr << node;
            cerr << "Nodes: " << m_expandedNodes << endl;
            cerr << "Time since beginning (sec): "
                    << getElapsedTime() << endl;
            cerr << "=============================================" << endl << endl;
        }
    }
#endif
    bool only_them = m_onlyThemWords;
    //if (node.getDepth() < 2)
    //    only_them = true;
    vector<Node> successors = node.generateSuccessors(m_bestSolScore,
            only_them, m_pruneWithBPs);
    m_bfStatsCol.addDataPoint(node.getDepth(), successors.size());
    m_allVisitedNodes += node.getNrAllSuccessors();
    m_visitedNodes += successors.size();
    m_nodesSinceLastRestart++;
    //if (m_expandedNodes == 1)
    //    cerr << "Time to process root in seconds: " << getElapsedTime() << endl;
    time_t current;
    time(&current);
    m_timeStatsCol.addDataPoint(node.getDepth(), difftime(current, start));
    node.setSubtreeHitRemoteDeadlock(false);

    int nr_succs = successors.size();
    for (int it = 0; it < nr_succs; it++) {
        if (g_pm.getDelayedStaticAnal())
            if (!successors[it].staticAnalAndTest(m_bestSolScore, g_pm.getPruneWithBPs()))
                continue;
        if (m_nodeThresholdForRestart >= 0) {
            if (m_nodesSinceLastRestart >= m_nodeThresholdForRestart) {
                if (node.getDepth() > 0)
                    return DONTCARE;
                else
                    m_nodesSinceLastRestart = 0;
            }
        }
        //cerr << "MAX DEV BUDGET: " << m_maxDevBudget << endl;
        if (m_nrDevs + it > m_discrepBudget)
            continue;
        m_nrDevs += it;
        int result = depthFirst(successors[it]);
        m_nrDevs -= it;
        //if (result == MATCH_DEADLOCK_DB) 
        //    node.setSkipDeadlockExtraction(true);
        // if the subtree has hit a remote deadlock pattern,
        // we cannot perform deadlock pattern extraction from this node
        if (successors[it].getSubtreeHitRemoteDeadlock())
            node.setSubtreeHitRemoteDeadlock(true);
        // if we are backjumping from a deadlock, check if we should
        // stop at the current node
        if (m_backjump == 1) {
            if (result == DEADLOCK)
                if (!node.backjumpHere(m_lastDeadlockNode))
                    return DEADLOCK;
        }
        // if a solution is found, and solution sampling is turned on,
        // backtrack directly to root,
        // as we don't want to compute endless variations
        // of the same solution
        if (result == SUCCESS) {
            if (m_maxNrSolutions > 0 && m_nrSolutionsFound >= m_maxNrSolutions) {
                return SUCCESS;
            }
            if (m_solutionSampling == 1 && node.getDepth() > 0)
                return SUCCESS;
        }
        if (resourceLimit()) {
            m_resourceLimit = true;
            return RESOURCE_LIMIT; // stop no matter what
        }
    }
    // Since:
    // 1. the grid is not complete yet, since isGoal() returned false; and
    // 2. all successors have been tried and none succeeded,
    // it means that this is a deadlocked partial grid.
    // If this is the root node, deaddlock means that the problem is unsolvable
    // (assuming the solver is complete).
    if (g_pm.getDBSize() > 0) {
        if (node.getDepth() > 0 && /*!node.getSkipDeadlockExtraction() && */
                !node.getSubtreeHitRemoteDeadlock()) {
            node.computeDeadlockInfo(successors);
            //cerr << "Deadlocked state" << endl;
            //cerr << node << endl;
            m_deadlocks.addRecord(node);
            m_lastDeadlockNode = node;
        }
    }
    if (node.getDepth() == 0 && m_nrSolutionsFound == 0) {
        if (!m_resourceLimit)
            m_nrSolutionsFound = -1; //problem was proven unsolvable (unless solver is incomplete)
        m_summaryInfo = getSummaryInfo();
    }
    return DEADLOCK;
}

void DepthFirstSearch::processNode(const Node & node) {
    int nr_them_points = node.getData().getNrThematicPoints();
    if (nr_them_points > this->m_bestSolScore) {
        this->m_bestSolScore = nr_them_points;
        this->m_bestPartSol = node;
    }
    if ((g_pm.getVerbosity() > 0 && nr_them_points >= this->m_bestSolScore) ||
    nr_them_points > this->m_bestSolScore) {
        stringstream msg;
        msg << "\n\033[1;31mNew partial solution with a high score found in DFS after " <<
            getElapsedTime() << " seconds" << reset << endl <<
            "Nr solutions: " << m_nrSolutionsFound <<
            " Exp nodes: " << m_expandedNodes <<
            " Visited nodes: " << m_visitedNodes << endl << node;
        cerr << msg.str();
    }
    if (node.isGoal()) {
        if (g_pm.getVerbosity() > 0)
            cerr << node << endl;
        if (m_solFileName.length() > 0) {
            ofstream ofile(m_solFileName.c_str(), ios::out | ios::app);
            node.getData().writeToFile2(ofile);
            ofile.close();            
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
        double ratio = 1.0 * nr_them_points / node.getData().getNrFilledCells();
        if (ratio > this->m_maxTSR) {
            this->m_maxTSR = ratio;
        }
    }
    if (m_verbosity >= 2 ||
            (m_expandedNodes % 10000 == 0 && m_verbosity >= 1)) {
        //if (m_maxNrSolutions < 0 || m_nrSolutionsFound < m_maxNrSolutions) {
        cerr << "Expanding node in DFS:" << endl;
        cerr << node;
        cerr << "Nodes: " << m_expandedNodes << endl;
        cerr << "Time since beginning (sec): "
                << getElapsedTime() << endl;
        cerr << "============================================="
                << endl << endl;
        //}
    }
}

bool DepthFirstSearch::resourceLimit() const {
    if (m_maxNrNodes >= 0 && m_expandedNodes > m_maxNrNodes)
        return true;
    if (m_maxTime >= 0 && getElapsedTime() >= m_maxTime)
        return true;
    return false;
}

bool DepthFirstSearch::foundAllSolutions() const {
    if (m_maxNrSolutions >= 0 && m_nrSolutionsFound >= m_maxNrSolutions)
        return true;
    return false;
}

int DepthFirstSearch::m_nrDevs;
int DepthFirstSearch::m_discrepBudget;
int DepthFirstSearch::m_sterileBudget;
int DepthFirstSearch::DONE;
