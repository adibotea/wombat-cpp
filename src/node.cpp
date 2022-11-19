/*
 *  node.cpp
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "node.h"
#include "booleanmap.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <list>
#include "util.h"

Node::Node()
:m_depth(0), m_whichChild(0),
m_branchingFactor(0), m_pathInfo(""), m_hitRemoteDP(false),
m_allSuccessors(0)
{}

Node::Node(const RandomWriteState & state, 
           unsigned int depth, unsigned int whichChild,
           const string & pathInfo)
:m_data(state), m_depth(depth), m_whichChild(whichChild),
m_branchingFactor(0), m_pathInfo(pathInfo), m_hitRemoteDP(false),
m_allSuccessors(0)
{
}

Node::Node(const RandomWriteState & state, 
           unsigned int depth, unsigned int whichChild)
:m_data(state), m_depth(depth), m_whichChild(whichChild), 
m_branchingFactor(0), m_pathInfo(""),
m_hitRemoteDP(false), m_allSuccessors(0)
{}

Node::~Node()
{}

void Node::computeDeadlockInfo(const vector<Node> & successors) {
    BooleanMap boolMap;
    for (int it = 0; it < successors.size(); it++) {
        boolMap.markCells(successors[it].getPartialDeadlock().getCells(), true);
    }
    m_data.computeDeadlockInfo(boolMap);
}

vector<Node> Node::generateSuccessors(int bestSolScore, 
        bool only_them_words, bool prune_with_bps) {
    vector<Node> result;
    unsigned int whichChild = 0;
    vector<RandomWriteState> states = m_data.generateSuccessors(bestSolScore,
            only_them_words, prune_with_bps);
    m_allSuccessors = m_data.getNrAllSuccessors();
    int newDepth = 0;
    if (states.size() >= 1)
        newDepth = getDepth() + 1;
    else
        newDepth = getDepth();
    for (vector<RandomWriteState>::iterator it = states.begin();
            it != states.end(); ++it) {
        stringstream newPathInfo;
        newPathInfo << it->getLastWord() << " ";
        newPathInfo << ++whichChild << ":" << states.size() << " ; ";
        string pathInfo = getPathInfo() + newPathInfo.str();
        result.push_back(Node(*it, newDepth, whichChild, pathInfo));
    }
    this->m_branchingFactor = states.size();
    return result;
}

bool Node::deadlockIsRemote(const vector<CellRecord> & deadlockCells) const {
    const vector<Cell> & slot = m_data.getRecentCellInstantiations();
    for (int it1 = 0; it1 < slot.size(); it1++) {
        for (int it2 = 0; it2 < deadlockCells.size(); it2++) {
            if (slot[it1].m_row == deadlockCells[it2].m_row &&
                slot[it1].m_col == deadlockCells[it2].m_col)
                return false;
        }
    }
    return true;
}

void Node::writeAsRbs(ostream &os) const
{
    const State & state = this->getData();
    os << "NO NAME" << endl;
    os << state.getNrRows() << endl;
    os << state.getNrCols() << endl;
    for (int i = 0; i < state.getNrRows(); i++) {
        string row = state.getRow(i);
        for (int j = 0; j < row.length(); j++)
            if (!isBlackPoint(row[j]))
                os << row[j];
            else
                os << '.';
        os << endl;
    }
    os << "1" << endl;
    os << "Grila generata automat cu programul Combus" << endl;
    for (int i = 0; i < state.getNrRows(); i++) {
        string row = state.getRow(i);
        os << row;
        if (i < state.getNrRows() - 1)
            os << "@";
        else os << endl;
    }
    for (int i = 0; i < state.getNrCols(); i++) {
        string col = state.getCol(i);
        os << col;
        if (i < state.getNrCols() - 1)
            os << "@";
    }
}

bool operator< (const Node &c1, const Node &c2)
{
    return c1.m_data < c2.m_data;
}

vector<string> Node::processPathInfo() const {
    vector<string> result;
    string s = m_pathInfo;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
        if (token.find(":") != std::string::npos)
            continue;
        if (token.find(";") != std::string::npos)
            continue;
        if (token.find("placeholder") != std::string::npos)
            continue;
        result.push_back(token);
    }
    if (s.find(":") == std::string::npos &&
        s.find("placeholder") == std::string::npos &&
        s.find(";") == std::string::npos)
        result.push_back(s);
    return result;
}