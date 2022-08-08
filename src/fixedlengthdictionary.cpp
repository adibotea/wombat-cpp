/*
 *  fixedlengthdictionary.cpp
 *  combus
 *
 *  Created by Adi Botea on 7/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "fixedlengthdictionary.h"

#include <algorithm>
#include <iterator>
#include <vector>
#include <list>
#include <assert.h>

FixLenDic::FixLenDic()
: m_length(0) {
}

FixLenDic::FixLenDic(int length)
: m_length(length) {
}

FixLenDic::~FixLenDic() {
}

void FixLenDic::addWord(const string & line, int globalIdx) {
    if (line.size() != m_length)
        assert(line.size() == m_length);
    //updatePrefixIndex(line, globalIdx);
    updateRegexIndex(line, globalIdx);
    m_words.push_back(line);
    m_allIndexes.push_back(globalIdx);
    assert(globalIdx + 1 == m_words.size());
}

void FixLenDic::updateRegexIndex(const string & line,
        int globalIdx) {
    for (int posIdx = 0; posIdx < line.length(); posIdx++) {
        int charIdx = (int) toupper(line[posIdx]) - 'A';
        m_regexIndex[charIdx][posIdx].push_back(globalIdx);
        //       m_pattern[charIdx][posIdx].addWordInfo(line);
    }
}

vector<string> FixLenDic::expandPattern(const WordPattern & pattern) const {
    vector<string> result;
    const string & regex = pattern.getSimplePattern();
    vector<int> tempVector1, tempVector2;
    bool seenFirstChar = false;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD)
            continue;
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        if (m_regexIndex[charIdx][posIdx].size() == 0) {
            assert(result.size() == 0);
            return result;
        }
        if (!seenFirstChar) {
            seenFirstChar = true;
            tempVector1 = m_regexIndex[charIdx][posIdx];
            continue;
        }
        set_intersection(tempVector1,
                m_regexIndex[charIdx][posIdx],
                tempVector2);
        tempVector1 = tempVector2;
        tempVector2.clear();
    }
    // if pattern contains only blanks
    if (!seenFirstChar) {
        if (pattern.hasNoConstraints())
            return m_words;
        int posIdx = pattern.getMostConstrainedPos();
        vector<int> chars = pattern.getTrueBits(posIdx);
        for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
            tempVector1 = m_regexIndex[chars[charIdx]][posIdx];
            for (int it = 0; it < tempVector1.size(); it++) {
                if (pattern.matchesWord(m_words[tempVector1[it]]))
                    result.push_back(m_words[tempVector1[it]]);
            }
        }
        return result;
    }
    for (int it = 0; it < tempVector1.size(); it++) {
        if (pattern.matchesWord(m_words[tempVector1[it]]))
            result.push_back(m_words[tempVector1[it]]);
    }
    return result;
}

vector<string> FixLenDic::expandPattern2(const WordPattern & pattern) const {
    vector<string> result;
    const string & regex = pattern.getSimplePattern();
    int min = 1000000;
    int min_idx = -1;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD)
            continue;
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        if (m_regexIndex[charIdx][posIdx].size() == 0) {
            assert(result.size() == 0);
            return result;
        }
        if (m_regexIndex[charIdx][posIdx].size() < min) {
            min = m_regexIndex[charIdx][posIdx].size();
            min_idx = posIdx;
        }
    }
    if (min_idx >= 0) {
        int charIdx = (int) toupper(regex[min_idx]) - 'A';
		const vector<int> & v = m_regexIndex[charIdx][min_idx];
        for (int kk = 0; kk < v.size(); kk++) {
            if (pattern.matchesWord(m_words[v[kk]]))
                result.push_back(m_words[v[kk]]);
        }
        return result;
    }
    // if pattern contains only blanks
	if (pattern.hasNoConstraints())
		return m_words;
	int posIdx = pattern.getMostConstrainedPos();
	vector<int> chars = pattern.getTrueBits(posIdx);
	for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
		const vector<int> & v = m_regexIndex[chars[charIdx]][posIdx];
		for (int it = 0; it < v.size(); it++) {
			if (pattern.matchesWord(m_words[v[it]]))
				result.push_back(m_words[v[it]]);
		}
	}
	return result;
}

bool FixLenDic::atLeast1Hit(const WordPattern & pattern) const {
    const string & regex = pattern.getSimplePattern();
    vector<int> tempVector1, tempVector2;
    bool seenFirstChar = false;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD)
            continue;
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        if (m_regexIndex[charIdx][posIdx].size() == 0) {
            return false;
        }
        if (!seenFirstChar) {
            seenFirstChar = true;
            tempVector1 = m_regexIndex[charIdx][posIdx];
            continue;
        }
        set_intersection(tempVector1,
                m_regexIndex[charIdx][posIdx],
                tempVector2);
        tempVector1 = tempVector2;
        tempVector2.clear();
    }
    // if pattern contains only blanks
    if (!seenFirstChar) {
        return (this->m_words.size() > 0);
    }
    for (int it = 0; it < tempVector1.size(); it++) {
        if (pattern.matchesWord(m_words[tempVector1[it]]))
            return true;
    }
    return false;
}

vector<int> FixLenDic::expandPatternIndex(const WordPattern & pattern) const {
    vector<int> result;
    const string & regex = pattern.getSimplePattern();
    vector<int> tempVector1, tempVector2;
    bool seenFirstChar = false;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD)
            continue;
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        if (m_regexIndex[charIdx][posIdx].size() == 0) {
            assert(result.size() == 0);
            return result;
        }
        if (!seenFirstChar) {
            seenFirstChar = true;
            tempVector1 = m_regexIndex[charIdx][posIdx];
            continue;
        }
        set_intersection(tempVector1,
                m_regexIndex[charIdx][posIdx],
                tempVector2);
        tempVector1 = tempVector2;
        tempVector2.clear();
    }

    if (!seenFirstChar) {
        if (pattern.hasNoConstraints())
            return m_allIndexes;
        int posIdx = pattern.getMostConstrainedPos();
        vector<int> chars = pattern.getTrueBits(posIdx);
        if (chars.size() >= 15 && false) {
            int nrWords = 0;
            for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
                nrWords += m_regexIndex[chars[charIdx]][posIdx].size();
            }
            //           return nrWords;
        } else {
            int posIdx = pattern.getMostConstrainedPos();
            vector<int> chars = pattern.getTrueBits(posIdx);
            for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
                tempVector1 = m_regexIndex[chars[charIdx]][posIdx];
                for (int it = 0; it < tempVector1.size(); it++) {
                    assert(tempVector1[it] < m_words.size());
                    if (pattern.matchesWord(m_words[tempVector1[it]]))
                        tempVector2.push_back(tempVector1[it]);
                }
            }
        }
    } else {
        if (pattern.hasNoConstraints()) {
            tempVector2 = tempVector1;
        } else {
            for (int it = 0; it < tempVector1.size(); it++) {
                if (pattern.matchesWord(m_words[tempVector1[it]]))
                    tempVector2.push_back(tempVector1[it]);
            }
        }
    }
    return tempVector2;


    // if pattern contains only blanks
    if (!seenFirstChar) {
        if (pattern.hasNoConstraints()) {
            return m_allIndexes;
        }
        int posIdx = pattern.getMostConstrainedPos();
        vector<int> chars = pattern.getTrueBits(posIdx);
        for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
            tempVector1 = m_regexIndex[chars[charIdx]][posIdx];
            for (int it = 0; it < tempVector1.size(); it++) {
                if (pattern.matchesWord(m_words[tempVector1[it]]))
                    result.push_back(tempVector1[it]);
            }
        }
        return result;
    }
    for (int it = 0; it < tempVector1.size(); it++) {
        if (pattern.matchesWord(m_words[tempVector1[it]]))
            result.push_back(tempVector1[it]);
    }
    return result;
}

/*
 Given a regular expression as a pattern, add to each position all letters
 that belong to at least one matching word (on the corresponding position).
 As a side effect, return the number of all words that match
 the pattern. This is useful to have both data with one method call.
 */
int FixLenDic::fillMultiCharPattern(WordPattern & pattern) const {
    vector<int> tempVector1, tempVector2;
    const string & regex = pattern.getSimplePattern();
    bool seenFirstChar = false;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD)
            continue;
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        assert(0 <= charIdx && charIdx <= 'Z' - 'A');
        if (m_regexIndex[charIdx][posIdx].size() == 0) {
            pattern.setAllMultiChar(false);
            return 0;
        }
        if (!seenFirstChar) {
            seenFirstChar = true;
            tempVector1 = m_regexIndex[charIdx][posIdx];
            continue;
        }
        set_intersection(tempVector1,
                m_regexIndex[charIdx][posIdx],
                tempVector2);
        tempVector1 = tempVector2;
        tempVector2.clear();
    }
    tempVector2.clear();
    // if pattern contains only wild cards
    if (!seenFirstChar) {
        int posIdx = pattern.getMostConstrainedPos();
        vector<int> chars = pattern.getTrueBits(posIdx);
        if (chars.size() >= 15) {
            int nrWords = 0;
            for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
                nrWords += m_regexIndex[chars[charIdx]][posIdx].size();
            }
            return nrWords;
        } else {
            int posIdx = pattern.getMostConstrainedPos();
            vector<int> chars = pattern.getTrueBits(posIdx);
            for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
                tempVector1 = m_regexIndex[chars[charIdx]][posIdx];
                for (int it = 0; it < tempVector1.size(); it++) {
                    assert(tempVector1[it] < m_words.size());
                    if (pattern.matchesWord(m_words[tempVector1[it]]))
                        tempVector2.push_back(tempVector1[it]);
                }
            }
        }
    } else {
        if (pattern.hasNoConstraints()) {
            tempVector2 = tempVector1;
        } else {
            for (int it = 0; it < tempVector1.size(); it++) {
                if (pattern.matchesWord(m_words[tempVector1[it]]))
                    tempVector2.push_back(tempVector1[it]);
            }
        }
    }
    pattern.setAllMultiChar(false);
    for (vector<int>::iterator it = tempVector2.begin();
            it != tempVector2.end(); it++) {
        for (int position = 0; position < regex.size(); position++) {
            int charIdx = (int) toupper(m_words[*it][position]) - 'A';
            pattern.setMultiCharValue(position, charIdx, true);
        }
    }
    return tempVector2.size();
}

unsigned int FixLenDic::getNrWordsMatchingPattern(const WordPattern & pattern) const {
    int result = 0;
    std::vector<int> tempVector1, tempVector2;
    const std::string & regex = pattern.getSimplePattern();
    bool seenFirstChar = false;
    for (int posIdx = 0; posIdx < regex.length(); posIdx++) {
        if (regex[posIdx] == WILDCARD) {
            continue;
        }
        int charIdx = (int) toupper(regex[posIdx]) - 'A';
        if (charIdx < 0) {
            cerr << "Char in reg ex: " << int(regex[posIdx]) << endl;
	    cerr << "Regex: " << regex << ";" << endl;
            exit(2);
        }
        assert(charIdx >= 0);
        if (m_regexIndex[charIdx][posIdx].size() == 0)
            return 0;
        if (!seenFirstChar) {
            seenFirstChar = true;
            tempVector1 = m_regexIndex[charIdx][posIdx];
            continue;
        }
        // else (second or later character)
        set_intersection(tempVector1,
                m_regexIndex[charIdx][posIdx],
                tempVector2);
        tempVector1 = tempVector2;
        tempVector2.clear();
    }
    // if pattern contains only wild cards
    if (!seenFirstChar) {
        if (pattern.hasNoConstraints())
        	return this->m_words.size();
        int posIdx = pattern.getMostConstrainedPos();
        vector<int> chars = pattern.getTrueBits(posIdx);
        if (chars.size() >= 15) {
            int nrWords = 0;
            for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
                nrWords += m_regexIndex[chars[charIdx]][posIdx].size();
            }
            return nrWords;
        }
        for (int charIdx = 0; charIdx < chars.size(); charIdx++) {
            tempVector1 = m_regexIndex[chars[charIdx]][posIdx];
            for (int it = 0; it < tempVector1.size(); it++) {
                if (pattern.matchesWord(m_words[tempVector1[it]]))
                    result++;
            }
        }
        return result;
    }
    for (int it = 0; it < tempVector1.size(); it++) {
        if (pattern.matchesWord(m_words[tempVector1[it]]))
            result++;
    }
    return result;
}

bool FixLenDic::sanityCheck(int value) const {
    for (int charIdx = 0; charIdx <= 'Z' - 'A'; charIdx++)
        for (int posIdx = 0; posIdx < m_length; posIdx++) {
            vector<int> v = m_regexIndex[charIdx][posIdx];
            for (int it = 0; it < v.size(); it++)
                if (v[it] < 0 || v[it] >= value)
                    return false;
        }
    return true;
}

void FixLenDic::set_intersection(const std::vector<int> & v1,
        const std::vector<int> & v2,
        std::vector<int> & result) const {
    result.clear();
    std::vector<int>::const_iterator it1 = v1.begin();
    std::vector<int>::const_iterator it2 = v2.begin();
    while (true) {
        if (it1 == v1.end() || it2 == v2.end())
            break;
        if (*it1 == *it2) {
            result.push_back(*it1);
            it1++;
            it2++;
        } else if (*it1 < *it2) {
            *it1++;
        } else if (*it2 < *it1) {
            *it2++;
        }
    }
}
