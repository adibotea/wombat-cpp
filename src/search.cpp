/*
 *  search.cpp
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "search.h"
#include <time.h>
#include "util.h"
#include <iostream>
#include <sstream>
#include "state.h"

using namespace std;

Search::Search()
:m_expandedNodes(0),
m_allVisitedNodes(0),
m_visitedNodes(0),
m_maxTime(-1.0),
m_maxNrNodes(-1),
m_nrSolutionsFound(0),
m_maxNrSolutions(-1)
{
    time(&m_start);
}

Search::Search(const Node & root, double maxTime, int maxNodes, int maxSolutions,
               int verbosity)
:m_root(root),
m_expandedNodes(0),
m_allVisitedNodes(0),
m_visitedNodes(0),
m_maxTime(maxTime),
m_maxNrNodes(maxNodes),
m_nrSolutionsFound(0),
m_maxNrSolutions(maxSolutions),
m_verbosity(verbosity)
{
    time(&m_start);
    m_c_start = std::clock();
}

Search::~Search()
{}

double Search::getElapsedTime2() const {
    std::clock_t c_end = std::clock();
    long double time_elapsed_ms = 1000.0 * (c_end - m_c_start) / CLOCKS_PER_SEC;
    return time_elapsed_ms / 1000.0;

}

double Search::getElapsedTime() const
{
    time_t current;
    time(&current);
    double result = difftime(current, m_start);
    assert (result >= 0.0);
    return result;
}

void Search::printSummary(ostream & os)
{
    os << getSummaryInfo();
}

string Search::getSummaryInfo() const
{
    stringstream result;
    result << g_pm.getInputFileName() 
            << " ### Nr solutions found: " << this->m_nrSolutionsFound //<< endl
            << " Best (partial) score: " << this->m_bestSolScore
            << " Target score: " << g_pm.getMinNrThemPoints()
            << " Elapsed time: " << this->getElapsedTime() //<< endl
            << " Expanded nodes: " << this->m_expandedNodes //<< endl
            << " Visited nodes: " << this->m_visitedNodes //<< endl
            //<< "    All visited nodes: " << this->m_allVisitedNodes << endl
            << " Average depth: " << this->m_depthStats.getAverage() //<< endl
            << " Max depth: " << this->m_depthStats.getMax(); //<< endl
            //<< " Average depth of solutions: " << this->m_solDepthStats.getAverage();
    return result.str();
}

void Search::printStats(ostream & os)
{
    os << " " << m_bfStatsCol << " " << m_timeStatsCol;
}
