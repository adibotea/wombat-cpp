/*
 *  fixedlengthdictionary.h
 *  combus
 *
 *  Created by Adi Botea on 7/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FIXED_LENGTH_DICTIONARY_H_
#define _FIXED_LENGTH_DICTIONARY_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "wordpattern.h"
#include "multicharmap.h"
#include "constants.h"

using namespace std;

class FixLenDic {
public:
    FixLenDic();
    FixLenDic(int length);
    ~FixLenDic();
    void addWord(const string & line, int globalIdx);
    /*
     * Return true if the dictionary has at least one word matching the pattern
     */
    bool atLeast1Hit(const WordPattern & pattern) const;
    /*
     * Given a pattern, return all words matching that pattern.
     * In particular, the returned words have the same length as the input pattern.
     */
    vector<string> expandPattern(const WordPattern & pattern) const;
    vector<string> expandPattern2(const WordPattern & pattern) const;
    vector<int> expandPatternIndex(const WordPattern & pattern) const;
    int fillMultiCharPattern(WordPattern & pattern) const;
    unsigned int getNrWordsMatchingPattern(const WordPattern & pattern) const;

    string getWord(int index) const {
        return m_words[index];
    };

    unsigned int size() const {
        return m_words.size();
    }
    bool sanityCheck(int value) const;
private:
    vector<int> m_regexIndex[NR_CHARS][MAX_WORD_LENGTH];
    vector<string> m_words;
    int m_length;
    MultiCharPattern m_pattern[NR_CHARS][MAX_WORD_LENGTH];
    vector<int> m_allIndexes; // a vector with property that m_allIndexes[i] = i
    void updateRegexIndex(const string & line,
            int globalIdx);
    void set_intersection(const vector<int> & v1,
            const vector<int> & v2,
            vector<int> & result) const;
};

#endif
