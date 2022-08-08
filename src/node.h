/*
 *  node.h
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _NODE_H_
#define _NODE_H_

#include "randomwritestate.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "deadlockrecord.h"
#include "globals.h"

class Node {
public:
    Node();
    Node(const RandomWriteState & state,
            unsigned int depth, unsigned int whichChild,
            const string & pathInfo);
    Node(const RandomWriteState & state,
            unsigned int depth, unsigned int whichChild);
    ~Node();

    bool backjumpHere(const Node & deadlockNode) const {
        return m_data.backjumpHere(deadlockNode.getData());
    }
    void computeDeadlockInfo(const vector<Node> & successors);

    const RandomWriteState getData() const {
        return m_data;
    }

    DeadlockRecord getDeadlockRecord() const {
        return m_data.getDeadlockCells();
    }

    DeadlockRecord getPartialDeadlock() const {
        return m_data.getPartialDeadlock();
    }

    int getDepth() const {
        return m_depth;
    }

    bool isGoal() const {
        return m_data.isGoal();
    }

    unsigned int getWhichChild() {
        return m_whichChild;
    }

    unsigned int getBranchingFactor() {
        return m_branchingFactor;
    }

    string getPathInfo() const {
        return m_pathInfo;
    }
    
    /*
     * Perform a static analysis of the state.
     * Return false iff a deadlock is detected.
     */
    bool staticAnalAndTest(double bestSolScore, bool prune_bps) {
        return this->m_data.staticAnalAndTest(bestSolScore, prune_bps);
    }
    
    vector<Node> generateSuccessors(int bestSolScore,
            bool only_them_words, bool prune_with_bps);

    bool getSkipDeadlockExtraction() const {
        return m_data.getSkipDeadlockExtraction();
    }

    void setSkipDeadlockExtraction(bool value) {
        m_data.setSkipDeadlockExtraction(value);
    }
    // return true if at least one node N in the subtree matches a deadlock
    // pattern that does not intersect the slot to be instantiated in node N.

    bool getSubtreeHitRemoteDeadlock() const {
        return m_hitRemoteDP;
    }

    void setSubtreeHitRemoteDeadlock(bool flag) {
        m_hitRemoteDP = flag;
    }

    void setRemoteDPInfo(const vector<CellRecord> & deadlockCells) {
        m_hitRemoteDP = deadlockIsRemote(deadlockCells);
    }
    bool deadlockIsRemote(const vector<CellRecord> & deadlockCells) const;

    int getNrAllSuccessors() const {
        return m_allSuccessors;
    }
    void writeAsRbs(ostream &os) const;
    friend bool operator<(const Node &c1, const Node &c2);

    std::vector<std::string> processPathInfo() const;

private:
    RandomWriteState m_data;
    unsigned int m_depth;
    unsigned int m_whichChild;
    unsigned int m_branchingFactor;
    string m_pathInfo;
    bool m_hitRemoteDP;
    int m_allSuccessors;
    // HashKey hashKey;
};

#endif
