/*
 *  wordslot.cpp
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "wordslot.h"
#include "state.h"
#include <algorithm>

using namespace std;

WordSlot::WordSlot(int row, int col, bool dir, const string &regex, int id)
: m_pattern(regex), m_row(row), m_col(col), m_direction(dir),
m_nrMatchingWords(-1), m_nrThemWords(-1), m_impactAreaSize(0), 
// m_domain, m_word_length
m_domainIsComputed(false), m_id(id),
// m_cells,
m_first_bp(1),
m_hasThemWords(false),
m_allowBPs(false), m_nonThem(false) {
    if (dir == HORIZONTAL) {
        for (int it = 0; it < regex.length(); it++) {
            m_cells.push_back(Cell(row, col + it));
        }
    } else {
        for (int it = 0; it < regex.length(); it++) {
            m_cells.push_back(Cell(row + it, col));
        }
    }
}

void WordSlot::setNrMatchingWords() {
    if (m_domain.size() == 0) {
        //m_nrMatchingWords = -1;
        return;
    }
    assert (m_domain.size() == 2);
    if (this->m_nonThem)
        m_nrMatchingWords = m_domain[1].size();
    else
        m_nrMatchingWords = m_domain[0].size();
}

void WordSlot::computeDomain(const vector<Dictionary> &dics, bool nonthemok) {
    m_domain.clear();
    for (vector<Dictionary>::const_iterator it = dics.begin(); it!= dics.end(); it++) {
        bool skip = false;
        if ((this->m_nonThem && it->isThematic()))
            skip = true;
        if (!nonthemok && !this->m_nonThem && !it->isThematic())
            skip = true;
//        bool skip = (this->m_nonThem && it->isThematic()) ||
//            (!nonthemok && !this->m_nonThem && !it->isThematic());
        if (skip) {
            vector<int> dummy;
            m_domain.push_back(dummy);
            continue;
        }
        const WordPattern & pattern = m_pattern;
        m_domain.push_back(it->expandPatternIndex(pattern));
    }
    assert (m_domain.size() == dics.size());
    m_domainIsComputed = true;
    setNrMatchingWords();
}

int WordSlot::computeImpactAreaSize(const vector<WordSlot> & slots) {
    m_impactAreaSize = 0;
    for (vector<WordSlot>::const_iterator it = slots.begin();
            it != slots.end(); it++) {
        if (it->getLength() < 3)
            continue;
        if (slotsCrossEachOther(*it))
            m_impactAreaSize += it->getLength();
    }
    return this->m_impactAreaSize;
}

void WordSlot::downwardsPropBPS(const vector<Dictionary> & dics, int first_blank_pos) {
    assert(dics.size() == m_domain.size());
    if (first_blank_pos <= 2)
        return;
    // Reset only the prefix, so that we recompute the domains of these prefix cells
    m_pattern.setMultiCharPrefix(first_blank_pos, false);
    for (int it = 0; it < dics.size(); it++) {
        for (int idx = 0; idx < m_domain[it].size(); idx++) {
            string word = dics[it].getWord((m_domain[it])[idx], (m_word_length[it])[idx]);
            for (int pos = 0; pos < first_blank_pos; pos++) {
                char charIdx = toupper(word[pos]) - 'A';
                m_pattern.setMultiCharValue(pos, charIdx, true);
            }
        }
    }
}

bool WordSlot::slotsCrossEachOther(const WordSlot & slot) const {
    if (this->getDirection() == slot.getDirection())
        return false;
    const WordSlot & horizSlot = (m_direction == HORIZONTAL) ? (*this) : slot;
    const WordSlot & vertSlot = (m_direction == VERTICAL) ? (*this) : slot;

    if (horizSlot.getRow() < vertSlot.getRow() ||
            horizSlot.getRow() >= vertSlot.getRow() + vertSlot.getLength())
        return false;

    if (vertSlot.getCol() < horizSlot.getCol() ||
            vertSlot.getCol() >= horizSlot.getCol() + horizSlot.getLength())
        return false;

    return true;
}

/*
Given a pattern, this method tells what characters are legal on
 the blank cells of the pattern.
 */
void WordSlot::updateMultiCharPatternFromDictionaries(const vector<Dictionary> & dics) {
    WordPattern finalPattern(m_pattern.getLength());
    finalPattern.setAllMultiChar(false);
    m_nrMatchingWords = 0;
    for (vector<Dictionary>::const_iterator it1 = (dics).begin();
            it1 != (dics).end(); it1++) {
        WordPattern pattern(m_pattern);
        m_nrMatchingWords += it1->fillMultiCharPattern(pattern);
        finalPattern.addMultiCharValues(pattern);
    }
    m_pattern.setAllMultiChar(false);
    m_pattern.addMultiCharValues(finalPattern);
}

void WordSlot::updateMultiCharPatternFromGridMap(const MultiCharMap & map) {
    m_pattern.updateMultiCharPatternFromGridMap(map, m_row, m_col,
            m_direction, getLength());
}

int WordSlot::getFirstBP() const {
    if (this->getLength() <= 2) {
        // Not sure what exactly should I do here.
        // Returning 0 is correct, but not sure if more effective values
        // can be set.
        return 0;
    } else {
        return this->m_first_bp;
    }
}

int WordSlot::getNrBlanks() const {
    int result = 0;
    for (int i = 0; i < this->getLength(); i++) {
        if (this->getLetter(i) == BLANK)
            result++;
    }
    return result;
}

bool WordSlot::good4TopLeftCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row > 3)
            return false;
        if (m_col > 0)
            return false;
    } else {
        if (m_col > 3)
            return false;
        if (m_row > 0)
            return false;
    }
    return true;
}

bool WordSlot::good4TopRightCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row > 3)
            return false;
        if (m_col + this->getLength() < 12)
            return false;
    } else {
        if (m_col < 10)
            return false;
        if (m_row > 0)
            return false;
    }
    return true;
}

bool WordSlot::good4BottomRightCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row < 7)
            return false;
        if (m_col + this->getLength() < 12)
            return false;
    } else {
        if (m_col < 7)
            return false;
        if (m_row + this->getLength() < 12)
            return false;
    }
    return true;
}

bool WordSlot::good4BottomLeftCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row < 9)
            return false;
        if (m_col > 0)
            return false;
    } else {
        if (m_col >= 4)
            return false;
        if (m_row + this->getLength() < 12)
            return false;
    }
    return true;
}

void WordSlot::setNrMatchingWords(int value) {
    m_nrMatchingWords = value;
}

void WordSlot::setNrMatchWordsToDom() {
    if (m_domain.size() == 0) {
        m_nrMatchingWords = -1;
        return;
    }
    m_nrMatchingWords = 0;
    m_nrThemWords = 0;
    for (int it = 0; it < m_domain.size(); it++) {
        m_nrMatchingWords += m_domain[it].size();
        if (it == 0)
            m_nrThemWords += m_domain[it].size();
    }

    //if (this->m_pattern.isBlank())
    //    cerr << "--- Setting nr matching words to " << this->m_nrMatchingWords << endl;
    // QUICK HACK TO COUNT THE POSSIBILITY OF PUTTING A BLACK POINT ON THE THIRD POSITION:
    //if (this->getLength() > 2 && this->getSimplePattern()[2] == BLANK)
    //    m_nrMatchingWords++;
}


void WordSlot::upwardsPropagation(const vector<Dictionary> & dics) {
    if (m_pattern.hasNoConstraints())
        return;
    assert (dics.size() == m_domain.size());
    vector<int> tmp;
    for (int it = 0; it < m_domain.size(); it++) {
        tmp.clear();
        for (int idx = 0; idx < m_domain[it].size(); idx++) {
            if (m_pattern.matchesWord(dics[it].getWord((m_domain[it])[idx], getLength()))) {
                tmp.push_back(m_domain[it][idx]);
            }
        }
        if (tmp.size() < m_domain[it].size()) {
            m_nrMatchingWords -= m_domain[it].size();
            m_nrMatchingWords += tmp.size();
            m_domain[it] = tmp;
        }
    }
}

void WordSlot::downwardsPropagation(const vector<Dictionary> & dics) {
    assert (dics.size() >= m_domain.size());
    m_pattern.setAllMultiChar(false);
    for (int it = 0; it < m_domain.size(); it++) {
        for (int idx = 0; idx < m_domain[it].size(); idx++) {
            string word = dics[it].getWord((m_domain[it])[idx], getLength());
            for (int pos = 0; pos < word.size(); pos++) {
                char charIdx = toupper(word[pos]) - 'A';
                m_pattern.setMultiCharValue(pos, charIdx, true);
            }
        }
    }
}

vector<int> WordSlot::getLetterBlockStarts() const {
    vector<int> result;
    for (int idx = 0; idx < this->getLength(); idx++) {
        if (this->getLetter(idx) != BLANK) {
            result.push_back(idx);
            while (idx < this->getLength() && this->getLetter(idx) != BLANK)
                idx++;
        }
    }
    return result;
}

double WordSlot::getStaticScore() const {
    if (m_nrMatchingWords <= 1)
        return m_nrMatchingWords;
    if (g_pm.getSlotScoring() == 0)
        return this->m_nrMatchingWords;
    if (g_pm.getSlotScoring() == 1)
        return 1.01 + (m_nrMatchingWords / (1 + m_nrThemWords * m_nrThemWords));
    int l2 = getLength() * getLength();
    if (g_pm.getSlotScoring() == 2)
        return 1.01 + (1.01 * m_nrMatchingWords - m_nrThemWords) / l2;
    if (g_pm.getSlotScoring() == 3)
        return 1.01 + (m_nrMatchingWords - m_nrThemWords);
    if (g_pm.getSlotScoring() == 4)
        return 1.0 + 1 / (1 + m_nrThemWords);
    if (g_pm.getSlotScoring() == 5)
        return 1.0 + ((double) m_nrMatchingWords) / l2; // original
    assert(false);
    return 1.0;
}

int WordSlot::getMinDistToBorder() const {
    int result = 0;
    if (this->getDirection() == HORIZONTAL) {
        result = (this->getRow() < MAX_NR_ROWS - this->getRow()) ? this->getRow() : (MAX_NR_ROWS - this->getRow());
    }
    if (this->getDirection() == VERTICAL) {
        result = (this->getCol() < MAX_NR_COLS - this->getCol()) ? this->getCol() : (MAX_NR_COLS - this->getCol());
    }
    return result;
}
