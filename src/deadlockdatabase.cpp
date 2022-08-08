/*
 *  deadlockdatabase.cpp
 *  combus
 *
 *  Created by Adi Botea on 1/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "deadlockdatabase.h"
#include "util.h"

bool DeadlockDB::nodeMatchesRecord(const Node & node) {
    const RandomWriteState & state = node.getData();
    for (auto it = m_db.begin(); it != m_db.end(); it++) {
        for (int idx = 3; idx < MAX_WORD_LENGTH; idx++)
        	if (node.getData().getNTW(idx) < it->getNTW(idx))
        		continue;
        if (state.matchesCellPattern(it->getCells())) {
        	return true;
            cerr << "State matches deadlock record" << endl;
            cerr << "Deadlock pattern:" << endl;
            cerr << *it << endl;
            cerr << "State:" << endl;
            cerr << state << endl;
            m_mostRecentHit = *it;
            return true;
        }
    }
    return false;
}
