/*
 *  dictionary.h
 *  combus
 *
 *  Created by Adi Botea on 1/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include "fixedlengthdictionary.h"
#include "wordpattern.h"
#include <vector>
#include "constants.h"
#include <unordered_map>
#include <assert.h>

using namespace std;

class Dictionary {
public:

	Dictionary(const string & filename, int maxWordLength, bool isThematic,
			int percentage);
	~Dictionary();
	void listWords(string prefix);
	/*
	 * Return true if the dictionary has at least one word matching the pattern
	 */
	bool atLeast1Hit(const WordPattern & pattern) const {
		return m_data[pattern.getLength()].atLeast1Hit(pattern);
	}

	/*
	 * Given a pattern, return all words matching that pattern.
	 * In particular, the returned words have the same length as the input pattern.
	 */
	vector<string> expandPattern(const WordPattern & pattern) const;
	vector<int> expandPatternIndex(const WordPattern & pattern) const;
	int fillMultiCharPattern(WordPattern & wordPattern) const;

	string getWord(int index, int length) const {
		if (length < 0 || length >= m_data.size())
			cerr << "Error " << length << endl;
		assert (0 <= length && length < m_data.size());
		return m_data[length].getWord(index);
	}
	;

	bool isThematic() const {
		return m_isThematic;
	}
	;
	unsigned int getNrWordsMatchingPattern(const WordPattern & pattern) const;
	int getWordId(const string & word) const {
		auto got = m_wordIds.find(word);
		if (got == m_wordIds.end())
			return -1;
		else
			return got->second;
	}
private:
	void addWord(const string & line, int globalIdx);
	int m_maxWordLength;
	bool m_isThematic;
	vector<FixLenDic> m_data;
	int m_percentage;
	std::unordered_map<std::string, int> m_wordIds;
};

#endif
