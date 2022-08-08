/*
 *  depthfirstsearch.h
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _DEPTHFIRSTSEARCH_H_
#define _DEPTHFIRSTSEARCH_H_

#include "search.h"
#include <iostream>
#include <fstream>
#include "deadlockdatabase.h"
#include "constants.h"

using namespace std;

class DepthFirstSearch : public Search
{
public:
    ~DepthFirstSearch();
    DepthFirstSearch(Node & root, double maxTime, int maxNodes, int maxSolutions,
                     string solFileName, string summaryFileName,
                     int verbosity, int backjump, int dbSize, int solSampling,
                     int threshold, bool only_them_words, bool prune_with_bps);
    DepthFirstSearch(Node & root, string summaryFileName);
    int runSearch();
    double getBestSolScore() const {
        return this->m_bestSolScore;
    }
    int getNrSolsFound() const {
        return this->m_nrSolutionsFound;
    }
    void clearDeadlockDB() {
        this->m_deadlocks.clear();
    }
    static void setMaxDevBudget(int val) {
        DepthFirstSearch::m_discrepBudget = val;
    }
    Node getLastProcessedNode() {
    	return this->m_lastProcessedNode;
    }
    Node getBestPartSol() const {
        return this->m_bestPartSol;
    }
private:
    string m_solFileName;
    string m_summaryFileName;
    string m_summaryInfo;
    Node m_lastDeadlockNode;
    int m_backjump;
    DeadlockDB m_deadlocks;
    /**
     * Last processed node.
     * Processed means to check goal condition and to expand it (if needed).
     **/
    bool m_resourceLimit;
    int m_solutionSampling;
    Node & m_lastProcessedNode;
    Node m_bestPartSol;
    int depthFirst(Node & node);
    void processNode(const Node & node);
    int m_nodeThresholdForRestart;
    int m_nodesSinceLastRestart;
    bool foundAllSolutions() const;
    bool resourceLimit() const;
    /*
     * Max thematic score ratio seen for any expanded node in this search.
     * The thematic score ratio is the number of thematic letters divided by
     * the number of filled cells.
     */
    float m_maxTSR;
    /*
     * True iff we use only thematic words in the grid
     */
    bool m_onlyThemWords;
    /*
     * True iff we use pruning with admissible estimates of future BPs
     */
    bool m_pruneWithBPs;
    
    static int m_nrDevs;
    static int m_discrepBudget;
    static int m_sterileBudget;
    static int DONE;
};

#endif
