/*
 *  deadlockdatabase.h
 *  combus
 *
 *  Created by Adi Botea on 1/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEADLOCK_DB_H_
#define _DEADLOCK_DB_H_

#include <iostream>
#include <list>
#include "deadlockrecord.h"
#include "util.h"
#include "node.h"

using namespace std;

class DeadlockDB {
public:

    DeadlockDB(int maxSize)
    : m_maxSize(maxSize) {
        this->m_db.clear();
    };

    void addRecord(const Node & node) {
    	//if (node.getData().getDeadlockCells().getCells().size() < 10)
    		addRecord(node.getData().getDeadlockCells());
    };

    void addRecord(const DeadlockRecord & record) {
        m_db.push_front(record);
        if (m_db.size() > m_maxSize)
            m_db.pop_back();
        if (g_pm.getVerbosity() >= 3) {
            cerr << "Adding deadlock pattern:" << endl;
            cerr << record << endl;
        }
    };

    const DeadlockRecord & getMostRecentHit() const {
        return m_mostRecentHit;
    }
    bool nodeMatchesRecord(const Node & node);

    void clear() {
        m_db.clear();
    }
protected:
    list<DeadlockRecord> m_db;
    int m_maxSize;
    DeadlockRecord m_mostRecentHit;
};

#endif
