/*
 *  dictionary.cpp
 *  combus
 *
 *  Created by Adi Botea on 1/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include "dictionary.h"
#include "globals.h"

using namespace std;

Dictionary::Dictionary(const string & filename, 
                       int maxWordLength, bool isThematic, int percentage)
:m_maxWordLength(maxWordLength),
m_isThematic(isThematic),
m_percentage(percentage)
{
    int wordid = 0;
    int index[MAX_WORD_LENGTH];
    string line;
    ifstream file(filename.c_str());
    for (int it = 0; it <= maxWordLength; it++) 
        index[it] = 0;
    m_data.reserve(maxWordLength + 1);
    for (int it = 0; it <= maxWordLength; it++) {
        FixLenDic dd(it);
        m_data.push_back(dd);
    }
    if (file.is_open()) {
        while (!file.eof()) {
            getline(file, line);
            if (line.length() <= maxWordLength) {
                if (line.length() == 0)
                    continue;
                string word = "";
                for (int it = 0; it < line.length(); it++) {
                    assert ('A' <= toupper(line[it]) && toupper(line[it]) <= 'Z');
                    word += toupper(line[it]);
                }
                if (rand()%100 < m_percentage) {
                    m_data[line.length()].addWord(word, index[line.length()]++);
                    if (this->m_isThematic)
                        this->m_wordIds[word] = wordid++;
                }
            }
        }
        file.close();
    } else {
        cerr << "Cannot open file " << filename << endl;
    }
    if (g_pm.getVerbosity() > 1)
		for (int it = 0; it < m_data.size(); it++) {
			cerr << "Dictionary with words of length " << it <<
			" has " << m_data[it].size() << " words" << endl;
		}
}

Dictionary::~Dictionary()
{}

vector<string> Dictionary::expandPattern(const WordPattern & pattern) const {
	if (g_pm.getDicSearchStyle() == 0)
		return m_data[pattern.getLength()].expandPattern(pattern);
	else
		return m_data[pattern.getLength()].expandPattern2(pattern);
}

vector<int> Dictionary::expandPatternIndex(const WordPattern & pattern) const {
    return m_data[pattern.getLength()].expandPatternIndex(pattern);
}

int Dictionary::fillMultiCharPattern(WordPattern & wordPattern) const {
    return m_data[wordPattern.getLength()].fillMultiCharPattern(wordPattern);
}

unsigned int Dictionary::getNrWordsMatchingPattern(const WordPattern & pattern) const {
    return m_data[pattern.getLength()].getNrWordsMatchingPattern(pattern);
}
