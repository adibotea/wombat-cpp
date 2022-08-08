/*
 *  wordpattern.cpp
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "wordpattern.h"
#include <assert.h>

WordPattern::WordPattern() {
}

WordPattern::WordPattern(int length)
: m_multiCharPattern(length) {
}

WordPattern::WordPattern(const string & regex)
: m_multiCharPattern(regex.size()),
m_simplePattern(regex) {
}

WordPattern::WordPattern(const WordPattern & pattern)
: m_multiCharPattern(pattern.getMultiCharPattern()),
m_simplePattern(pattern.getSimplePattern()) {
}

WordPattern::~WordPattern() {
}

int WordPattern::getLength() const {
    return m_simplePattern.size();
}

bool WordPattern::hasNoConstraints() const {
    return m_multiCharPattern.hasNoConstraints();
}

bool WordPattern::isBlank() const {
    string simplePattern = getSimplePattern();
    for (int it = 0; it < simplePattern.size(); it++)
        if (simplePattern[it] != BLANK)
            return false;
    return true;
}

bool WordPattern::isInstantiated() const {
    return (this->getSimplePattern().find(' ') == string::npos);
}

void WordPattern::updateMultiCharPatternFromGridMap(const MultiCharMap & map,
        int row, int col,
        bool direction, int length) {
    m_multiCharPattern.updateFromGridMap(map, row, col, direction, length);
}

void WordPattern::addMultiCharValues(const WordPattern & pattern) {
    m_multiCharPattern.addMultiCharValues(pattern.getMultiCharPattern());
}

WordPattern WordPattern::getPrefix(int length) const {
    WordPattern result(length);
    assert(length <= this->m_simplePattern.length());
    result.m_multiCharPattern = this->m_multiCharPattern.getPrefix(length);
    result.m_simplePattern = this->m_simplePattern.substr(0, length);
    return result;
}

WordPattern WordPattern::getSubpattern(int start, int length) const {
    WordPattern result(length);
    assert(length <= this->m_simplePattern.length());
    result.m_multiCharPattern = this->m_multiCharPattern.getSubpattern(start, length);
    result.m_simplePattern = this->m_simplePattern.substr(start, length);
    assert(result.m_simplePattern.size() == length);
    return result;
}
