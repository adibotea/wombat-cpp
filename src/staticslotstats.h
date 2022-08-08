/*
 *  staticslotstats.h
 *  combus
 *
 *  Created by Adi Botea on 26/09/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STATICSLOTSTATS_H_
#define _STATICSLOTSTATS_H_

#include "stats.h"

class StaticSlotStats {
public:
    StaticSlotStats(){};
    ~StaticSlotStats(){};
    void addMatchingWordsRecord(int nrMatchingWords) {
        m_matchingWordsStats.addDataPoint((double)nrMatchingWords);
    }
    double getStatisticScore() {
        return m_scoreStats.getAverage();
    }
    int getStatisticVolume() {
        return m_scoreStats.getVolume();
    }
    void addScoreRecord(double score) {
        m_scoreStats.addDataPoint(score);
    }
private:
    Statistics m_matchingWordsStats;
    Statistics m_scoreStats;
};

#endif
