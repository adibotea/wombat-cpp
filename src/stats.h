/*
 *  stats.h
 *  combus
 *
 *  Created by Adi Botea on 28/06/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STATS_H_
#define _STATS_H_

#include <iostream>

using namespace std;

class Statistics {
public:
    Statistics()
    :m_name(""), m_volume(0), m_sum(0.0), m_sum2(0.0), m_min(32000.0), m_max(-32000.0), m_var(0)
    {}
    Statistics(string name)
    :m_name(name), m_volume(0), m_sum(0.0), m_sum2(0.0), m_min(32000.0), m_max(-32000.0), m_var(0)
    {}
    ~Statistics(){}
    void addDataPoint(double val) {
        m_sum += val;
        m_sum2 += val*val;
        m_volume++;
        if (m_min > val)
            m_min = val;
        if (m_max < val)
            m_max = val;
    }
    double getAverage() const {
        if (m_volume == 0)
            return 0;
        return m_sum/m_volume;
    }
    double getVariance() const {
        if (m_volume == 0)
            return 0;
        return m_sum2/m_volume - getAverage()*getAverage();
    }
    double getMin() const {
        return m_min;
    }
    double getMax() const {
        return m_max;
    }
    int getVolume() const {
        return m_volume;
    }
    string getName() const {
        return m_name;
    }
private:
    string m_name;
    int m_volume;
    double m_sum;
    double m_sum2;
    double m_min;
    double m_max;
    double m_var;
};

#endif
