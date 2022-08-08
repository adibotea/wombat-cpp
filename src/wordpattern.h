/*
 *  wordpattern.h
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _WORDPATTERN_H_
#define _WORDPATTERN_H_

#include <iostream>
#include <vector>
#include "multicharpattern.h"
#include "constants.h"

using namespace std;

/* Should make simplePattern and multiCharPattern consistent
   on the instantiated positions */

class WordPattern
{
public:
    WordPattern();
    WordPattern(int length);
    WordPattern(const string & regex);
    WordPattern(const WordPattern & pattern);
    ~WordPattern();
    void addMultiCharValues(const WordPattern & pattern);
    const string & getSimplePattern() const {
        return m_simplePattern;
    };
    string getSimplePatternDots() const {
        string res = m_simplePattern;
        for (int i = 0; i < res.length(); i++)
            if (res[i] == ' ')
                res[i] = '.';
        return res;
    }
    const MultiCharPattern & getMultiCharPattern () const {
        return m_multiCharPattern;
    };
    int getLength() const;
    /*
     * Get the first position that is blank in this slot.
     * If no position is blank (i.e., the slot is fully instantiated),
     * return the length of the slot.
     */
    int firstBlankPos() const {
        for (int i = 0; i < this->getLength(); i++) {
            if (this->m_simplePattern[i] == BLANK)
                return i;
        }
        return this->getLength();
    }
    int getMostConstrainedPos() const {
        return m_multiCharPattern.getMostConstrainedPos();
    };
    bool getMultiCharValue(int pos, int charIdx) const
    {
        return m_multiCharPattern.get(pos, charIdx);
    };
    vector<int> getTrueBits(int pos) const {
        return m_multiCharPattern.getTrueBits(pos);
    };
    void setMultiCharValue(int pos, int charIdx, bool value)
    {
        m_multiCharPattern.set(pos, charIdx, value);
    };
    void updateMultiCharPatternFromGridMap(const MultiCharMap & map,
                                           int row, int col, bool direction,
                                           int length);
    bool matchesWord(const string & word) const
    {
        if (m_simplePattern == word) 
            return true;
        for (int it = 0; it < m_simplePattern.length(); it++) {
        	if (m_simplePattern[it] != WILDCARD && m_simplePattern[it] != word[it])
        		return false;
        }
        return m_multiCharPattern.matchesWord(word);
    };
    // true if m_multiCharPattern has no constraints
    bool hasNoConstraints() const;
    bool isBlank() const;
    bool isInstantiated() const;
    void setAllMultiChar(bool value) {
        m_multiCharPattern.setAll(value);
    };
    void setMultiCharPrefix(int position, bool value) {
        m_multiCharPattern.resetPrefix(position, value);
    };
    void setPosition(int pos, char c) {
        m_simplePattern[pos] = c;
    };
    void setSimplePattern(const string & pattern) {
        m_simplePattern = pattern;
    };
    /*
     * Build and return a new WordPattern object.
     * The result is obtained from this by keeping a prefix
     * whose length is given as an argument.
     */
    WordPattern getPrefix(int length) const;
    /*
     * Build and return a new pattern, which is a fragment of this,
     * starting at position given in the first argument 
     * and having a length given in the second argument.
     * Example: If this is ...AMK... ,
     * getSubpattern(3, 5) will return AMK..
     */
    WordPattern getSubpattern(int first, int length) const;
private:
    MultiCharPattern m_multiCharPattern;
    string m_simplePattern;
};

#endif
