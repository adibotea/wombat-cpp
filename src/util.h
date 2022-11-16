/*
 *  util.h
 *  combus
 *
 *  Created by Adi Botea on 7/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include "state.h"
#include "node.h"
#include "stats.h"
#include "statscollection.h"
#include "deadlockrecord.h"
#include <string>
#include "move.h"

using namespace std;

ostream & operator << (ostream &os, const State & state);
ostream & operator << (ostream &os, const Node & node);
ostream & operator << (ostream &os, const Move & move);
// display in .rbs style
//ostream & operator < (ostream &os, const Node & node);
ostream & operator << (ostream &os, const Statistics & stats);
ostream & operator << (ostream &os, Statistics & stats);
ostream & operator << (ostream &os, const StatsCollection & col);
ostream & operator << (ostream &os, const DeadlockRecord & record);
//bool operator< (const State & s1, const State & s2);
bool operator== (const WordSlot &s1, const WordSlot &s2);
bool operator== (WordSlot &s1, WordSlot &s2);

struct IDPair {
    int int_member;
    double dbl_member;
     bool operator<(const IDPair &rhs) const {
         return this->dbl_member < rhs.dbl_member; 
     }
};

std::string tolower(std::string & input);

bool isBlackPoint(char c);

#endif
