/*
 *  depthfirstsearch.h
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _BESTFIRSTSEARCH_H_
#define _BESTFIRSTSEARCH_H_

#include "search.h"
#include <iostream>
#include <fstream>
#include "deadlockdatabase.h"
#include "constants.h"

using namespace std;

class BestFirstSearch : public Search
{
public:
    ~BestFirstSearch();
    BestFirstSearch(Node & root, double maxTime, int maxNodes, int maxSolutions,
                     string solFileName, string summaryFileName,
                     int verbosity, int backjump, int dbSize, int solSampling,
                     int threshold, bool only_them_words, bool prune_with_bps);
    BestFirstSearch(Node & root, string summaryFileName);
    int runSearch();
    double getBestSolScore() const {
        return this->m_bestSolScore;
    }
    int getNrSolsFound() const {
        return this->m_nrSolutionsFound;
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
    /**
     * Last processed node.
     * Processed means to check goal condition and to expand it (if needed).
     **/
    bool m_resourceLimit;
    int m_solutionSampling;
    Node & m_lastProcessedNode;
    Node m_bestPartSol;
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

    int time_to_report;
    
    static int m_sterileBudget;
    static int DONE;
};

#endif
