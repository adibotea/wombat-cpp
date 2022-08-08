/*
 *  wordslot.h
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _WORDSLOT_H_
#define _WORDSLOT_H_

#include <iostream>
#include "wordpattern.h"
#include "multicharmap.h"
#include "dictionary.h"
#include "stats.h"
#include <assert.h>
#include "constants.h"
#include "globals.h"

using namespace std;

class WordSlot {
public:

    WordSlot(int row, int col, bool direction, const string & pattern, int id);

    ~WordSlot() {
    };
    /*
     * Compute the domain by taking into account words of various lengths,
     * up to the length of the slot.
     * After the return, the last arguments is true iff the domain of this
     * slot has at least one thematic word
     * 
     */
    void computeDomainDBP(const std::vector<Dictionary> &dics,
            bool non_them_ok,
            vector<int> & legal_bps, bool bp_allowed);

    void computeDomain(const vector<Dictionary> &dics, bool nonthemok);

    void setNrMatchingWords();

    void setSterileFlag(bool flag) {
        this->m_nonThem = flag;
    }

    bool getSterileFlag() const {
        return this->m_nonThem;
    }

    void clearDomain() {
        m_domain.clear();
        m_domainIsComputed = false;
    };
    int computeImpactAreaSize(const vector<WordSlot> & slots);

    bool domainIsComputed() const {
        return m_domainIsComputed;
    };

    vector<int> getLetterBlockStarts() const;
    vector<WordSlot> getSubslotsAroundPos(int pos) const;
    /*
     * Split this into one or two slots based on a black point present inside this. 
     * The number of slots depends on the position of the black point.
     * The arguments are the row and the column of black point.
     * The number of slots depends on the position of the black point.
     * E.g., black point on the first or the last position lead to only one slot.
     */
    vector<WordSlot> split(int bp_row, int bp_col) const;
    /*
     * Compute and return a sub-slot of this, starting from the 
     * position given in the first argument and having a length given 
     * in the second argument.
     * No value is assigned to the m_id field.
     */
    WordSlot getSubslot(int start, int length) const;
    
    bool needsBP(vector<Dictionary> & dics) const;

    bool compatWithCornerSel() const {
        switch (g_pm.getCorner())  {
            case 0:
                return true;
                break;
            case 1:
                return this->good4TopLeftCorner();
                break;
            case 2:
                return this->good4TopRightCorner();
                break;
            case 3:
                return this->good4BottomRightCorner();
                break;
            case 4:
                return this->good4BottomLeftCorner();
                break;
        }
        return true;
    }

    bool good4BottomRightCorner() const;
    bool good4TopLeftCorner() const;
    bool good4BottomLeftCorner() const;
    bool good4TopRightCorner() const;
    bool containsCell(int row, int col) const {
        if (this->m_direction == HORIZONTAL)
            return (row == m_row &&
                m_col <= col &&
                col < m_col + this->getLength());
        else
            return (col == m_col &&
                m_row <= row &&
                row < m_row + this->getLength());
    }
    /*
     * Version of downwards propagation that works with DBPs.
     * Propagation from the slot level to the cell level.
     * Steps: 
     * 1. Reset the multi-char pattern.
     * 2. For each word in the domain of the slot, and for each letter 
     * in that word, set accordingly the flag in the multi-char pattern.
     */
    void downwardsPropBPS(const vector<Dictionary> & dics, int first_blank_pos);

    const string & getSimplePattern() const {
        return m_pattern.getSimplePattern();
    };
    
    char getLetterExt(int pos) const {
        if (pos == -1 || pos == this->getLength())
            return BLACKPOINT;
        return getLetter(pos);
    }

    bool getDirection() const {
        return m_direction;
    };

    void setAllMultiChar(bool value) {
        this->m_pattern.setAllMultiChar(value);
    };

    /*
     * Return the position (index) of the first cell of the slot
     * where we could place a black point.
     * E.g., if the slot pattern is FR....., return 4, as FRIG is the 
     * shortest valid word matching a prefix of the pattern.
     */
    int getFirstBP() const;
    void setNrMatchingWords(int value);
    /*
     * Set the number of matching words according to the domain size.
     */
    void setNrMatchWordsToDom();

    int getNrMatchingWords() const {
        return m_nrMatchingWords;
    };

    int getNrThemWords() const {
        return this->m_nrThemWords;
    }

    int getRow() const {
        return m_row;
    };

    int getCol() const {
        return m_col;
    };

    int getCellRow(int pos) const {
        return (m_direction == HORIZONTAL) ? (m_row) : (m_row + pos);
    };

    int getCellCol(int pos) const {
        return (m_direction == HORIZONTAL) ? (m_col + pos) : (m_col);
    };

    int getId() const {
        return m_id;
    };

    void setId(int val) {
        this->m_id = val;
    }

    int getLength() const {
        return m_pattern.getLength();
    };

    /*
     * Return the number of blanks in this slot
     */
    int getNrBlanks() const;
    
    const WordPattern & getPattern() const {
        return m_pattern;
    };

    const vector<Cell> & getCells() const {
        return m_cells;
    };

    bool getMultiCharValue(int pos, int charIdx) const {
        return m_pattern.getMultiCharValue(pos, charIdx);
    };

    char getLetter(int pos) const {
        const string & pattern = getSimplePattern();
        assert(0 <= pos && pos < pattern.length());
        return pattern[pos];
    }

    /*
     * For each solid block of letters, count the number of matching
     * words containing the block.
     * Pick the block with the minimum number of matching words.
     * Return the index of the starting position of the selected block,
     * as well as the corresponding number of words.
     */
    void getMinMatchingWords(int & block_start, int & nr_words, const vector<Dictionary> & dics);

    double getStaticScore() const;
   
    bool hasThemWords() const {
        return this->m_hasThemWords;
    }

    bool slotsCrossEachOther(const WordSlot & slot) const;
    void updateMultiCharPatternFromDictionaries(const vector<Dictionary> & dics);
    void updateMultiCharPatternFromGridMap(const MultiCharMap & map);

    void setPattern(const string & pattern) {
        m_pattern.setSimplePattern(pattern);
        //        m_pattern.getMultiCharPattern().setAll(true);
    }
    /*
     * Propagation from the cell level to the slot level.
     * I.e., restrict the domain of a slot (list of acceptable words)
     * only to those words that match the multi-char pattern.
     * Looks like the implementation works for DBPs as well.
     * This method directly updates the m_domain field.
     */
    void upwardsPropagation(const vector<Dictionary> & dics);
    void downwardsPropagation(const vector<Dictionary> & dics);
    /*
     * Considers the largest solid block of consecutive letters in this slot.
     * This function returns true if that block causes a deadlock.
     * E.g., ...MTS... would be a deadlock, as no word from the dictionary
     * contains the sequence MTS.
     */
    bool hasSolidBlockDeadlock(const vector<Dictionary> & dics,
            bool non_them_ok,
            vector<int> & legal_bps) const;
        
    /*
     * Return true iff the idx-th cell of this slot is a forced bp
     */
    bool isForcedBP(const vector<Dictionary> & dics,
            vector<int> & legal_bps, int idx) const;   
    
    bool cellAfterExists(int nr_rows, int nr_cols) const {
        if (this->getDirection() == HORIZONTAL) {
            return (this->getCol() + this->getLength() < nr_cols);
        } else
            return (this->getRow() + this->getLength() < nr_rows);
    }
    bool cellBeforeExists() const {
        if (this->getDirection() == HORIZONTAL) {
            return (this->getCol() > 0);
        } else
            return (this->getRow() > 0);
    }
    void setAllowBPs(bool value) {
    	this->m_allowBPs = value;
    }
    bool getAllowBPs() const {
    	return this->m_allowBPs;
    }
    int getMinDistToBorder() const;
private:
    WordPattern m_pattern;
    int m_row;
    int m_col;
    bool m_direction;
    int m_nrMatchingWords; // total nr words matching this slot
    int m_nrThemWords; // nr thematic words matching this slot
    int m_impactAreaSize; // all cells of the slots that intersect the current slot
    // all words that match this slot. we need one vector for each dictionary of the slot
    vector< vector<int> > m_domain;
    // the length of each word encoded in m_domain
    vector< vector<int> > m_word_length;
    bool m_domainIsComputed;
    int m_id;
    vector<Cell> m_cells;
    /*
     * Index of the first position where a black point could be placed.
     */
    int m_first_bp;
    /*
     * True iff the domain has at least one thematic word
     */
    bool m_hasThemWords;
    bool m_allowBPs;
    /*
     * When true, instantiate with non-thematic words.
     * Otherwise, instantiate with thematic words.
     */
    bool m_nonThem;
};

#endif
