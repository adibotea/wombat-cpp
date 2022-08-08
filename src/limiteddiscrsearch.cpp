/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IterLimDev.cpp
 * Author: adi
 * 
 * Created on 03 January 2018, 14:01
 */

#include "limiteddiscrsearch.h"
#include "depthfirstsearch.h"

LimitedDiscrSearch::LimitedDiscrSearch(int mb) {
    this->m_maxBudget = mb;
}

LimitedDiscrSearch::~LimitedDiscrSearch() {
}

void LimitedDiscrSearch::run(Node & root) {
    cerr << "Running Limited Discrepancy Search with DFS" << endl;
    cerr << "Max discrepancy budget = " << this->m_maxBudget << endl;
    int budget = 16;
    while (budget <= this->m_maxBudget) {
        Node rootcopy = Node(root.getData(), 0, 0);
        cerr << "Budget = " << budget << endl;
        DepthFirstSearch search(rootcopy,
                300, //g_pm.getMaxTime(),
                g_pm.getMaxNodes(),
                1, //g_pm.getMaxSolutions(),
                g_pm.getSolFileName(),
                "summary.txt",
                g_pm.getVerbosity(),
                g_pm.getBackjump(),
                g_pm.getDBSize(),
                g_pm.getSolutionSampling(),
                g_pm.getNodeThresholdForRestart(),
                false, // false = use both thematic and non-thematic words
                g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs 
        search.setMaxDevBudget(budget);
        search.runSearch();
        this->m_lastProcessedNode = search.getLastProcessedNode();
        if (search.getNrSolsFound() > 0) {
            cerr << "Return here from ILD" << endl;
            return;
        }
        budget *= 2;
    }
}
 
