/*
 *  multicharslot.cpp
 *  combus
 *
 *  Created by Adi Botea on 18/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "multicharpattern.h"
#include <iostream>
#include <assert.h>

MultiCharPattern::MultiCharPattern()
: m_length(0) {
    for (int pos = 0; pos < MAX_NR_POSITIONS; pos++) {
        m_nrTrueBits[pos] = 0;
        m_multiCharMap[pos] = 0;
    }
    setAll(true);
}

MultiCharPattern::MultiCharPattern(int length)
: m_length(length) {
    for (int pos = 0; pos < MAX_NR_POSITIONS; pos++) {
        m_nrTrueBits[pos] = 0;
        m_multiCharMap[pos] = 0;
    }
    setAll(true);
}

MultiCharPattern::~MultiCharPattern() {
}

void MultiCharPattern::addWordInfo(string word) {
    for (int pos = 0; pos < word.length(); pos++) {
        int charIdx = (int) (word[pos] - 'A');
        set(pos, charIdx, true);
    }
}

bool MultiCharPattern::get(int position, int c) const {
    assert(0 <= c && c <= 'Z' - 'A');
    int mask = 1 << (c);
    return (m_multiCharMap[position] & mask) ? true : false;
}

int MultiCharPattern::getMostConstrainedPos() const {
    int min = 30000;
    int minPos = -1;
    assert(0 <= m_length && m_length <= 'Z' - 'A' + 1);
    for (int pos = 0; pos < m_length; pos++) {
        if (m_nrTrueBits[pos] < min) {
            min = m_nrTrueBits[pos];
            minPos = pos;
        }
    }
    assert(minPos >= 0);
    return minPos;
}

vector<int> MultiCharPattern::getTrueBits(int position) const {
    vector<int> result;
    for (int c = 0; c < 'Z' - 'A' + 1; c++) {
        if (get(position, c))
            result.push_back(c);
    }
    return result;
}

void MultiCharPattern::set(int position, int c, bool value) {
    assert(0 <= c && c <= 'Z' - 'A');
    bool oldValue = get(position, c);
    if (oldValue != value) {
        int mask = (1 << c);
        m_multiCharMap[position] ^= mask;
        if (value)
            m_nrTrueBits[position]++;
        else
            m_nrTrueBits[position]--;
        assert(m_nrTrueBits[position] >= 0 &&
                m_nrTrueBits[position] <= 'Z' - 'A' + 1);
    }
}

void MultiCharPattern::resetPrefix(int position, bool value) {
    assert (position <= this->m_length);
    for (int idx = 0; idx < position; idx++) {
        for (int c = 0; c < 'Z' - 'A' + 1; c++) {
            set(position, c, value);
        }
    }
}

void MultiCharPattern::setAll(bool value) {
    for (int position = 0; position < m_length; position++)
        for (int c = 0; c < 'Z' - 'A' + 1; c++) {
            set(position, c, value);
        }
}

void MultiCharPattern::setAll(int pos, bool value) {
	for (int c = 0; c < 'Z' - 'A' + 1; c++) {
		set(pos, c, value);
	}
}


bool MultiCharPattern::matchesWord(const string & word) const {
    //    cerr << word << endl;
    for (int position = 0; position < word.length(); position++) {
        char c = toupper(word[position]);
        if (!get(position, c - 'A'))
            return false;
    }
    return true;
}

void MultiCharPattern::updateFromGridMap(const MultiCharMap & map,
        int row, int col, bool direction,
        int length) {
    for (int position = 0; position < length; position++) {
        for (int charIdx = 0; charIdx < 'Z' - 'A' + 1; charIdx++) {
            bool newValue;
            if (direction == HORIZONTAL)
                newValue = map.get(row, col + position, charIdx);
            else
                newValue = map.get(row + position, col, charIdx);
            set(position, charIdx, newValue);
        }
    }
}

void MultiCharPattern::addMultiCharValues(const MultiCharPattern & pattern) {
    for (int position = 0; position < m_length; position++) {
        for (int c = 0; c < 'Z' - 'A' + 1; c++) {
            bool newValue = get(position, c) || pattern.get(position, c);
            set(position, c, newValue);
        }
    }
}

bool MultiCharPattern::hasNoConstraints() const {
    if (m_nrTrueBits[getMostConstrainedPos()] == 'Z' - 'A' + 1)
        return true;
    return false;
}

MultiCharPattern MultiCharPattern::getPrefix(int length) const {
    MultiCharPattern result = *this;
    result.m_length = length;
    assert(result.m_length >= 1);
    return result;
}

MultiCharPattern MultiCharPattern::getSubpattern(int start, int length) const {
    MultiCharPattern result;
    assert (0 <= start && start < this->m_length);
    assert (start + length <= this->m_length);
    for (int idx = start; idx < start + length; idx++) {
        result.m_multiCharMap[idx - start] = this->m_multiCharMap[idx];
        result.m_nrTrueBits [idx - start] = this->m_nrTrueBits[idx];
    }
    result.m_length = length;
    return result;
}
