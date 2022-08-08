/*
 *  statscollection.h
 *  combus
 *
 *  Created by Adi Botea on 28/06/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STATSCOLLECTION_H_
#define _STATSCOLLECTION_H_

#include "stats.h"
#include <map>
#include <stdio.h>

class StatsCollection {
public:
    StatsCollection(){};
    ~StatsCollection(){};
    void addDataPoint(int index, double value) {
        if(m_collection.find(index) == m_collection.end()) {
            char name[10];
            sprintf(name, "%d", index);
            m_collection[index] = Statistics(name);
        }
        m_collection[index].addDataPoint(value);
    };
    const std::map<int, Statistics> & getMap() const {
        return m_collection;
    }
private:
    std::map<int, Statistics> m_collection;
};

#endif
