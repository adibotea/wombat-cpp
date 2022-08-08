/*
 *  multicharslot.h
 *  combus
 *
 *  Created by Adi Botea on 18/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MULTICHARPATTERN_H_
#define _MULTICHARPATTERN_H_

#include <vector>
#include "multicharmap.h"
#include <string>
#include "constants.h"

using namespace std;

class MultiCharPattern
{
public:
    MultiCharPattern();
    MultiCharPattern(int length);
    ~MultiCharPattern();
    bool get(int position, int c) const;
    int getMostConstrainedPos() const;
    std::vector<int> getTrueBits(int position) const;
    MultiCharPattern intersection(const MultiCharPattern & slot) const;
    void set(int position, int c, bool value);
    void setAll(int pos, bool value);
    void setAll(bool value);
    /*
     * Reset the elements before the argument to the given value
     */
    void resetPrefix(int position, bool value);
    void setLength(int length) {
        m_length = length;
    };
    /*
     * Return true if the word given as an argument matches into this slot.
     * It returns true even if the length of the word is smaller
     * than the length of the slot, as long as the letters of the 
     * word are consistent with the constraints for the corresponding slot cells.
     */
    bool matchesWord(const string & word) const;
    void updateFromGridMap(const MultiCharMap & map,
                           int row, int col, bool direction, int length);
    void addMultiCharValues(const MultiCharPattern & pattern);
    void addWordInfo(const string word);
    bool positionRecentlyConstrained(int position);
    bool hasNoConstraints() const;
    /*
     * Build and return a new object from this.
     * The result is a prefix of this object.
     * The length of the prefix is given as an argument.
     */
    MultiCharPattern getPrefix(int length) const;
    /*
     * Build and return a new object that is a subpattern of this,
     * starting at the position given in the first argument 
     * and having the length given in the second argument.
     */
    MultiCharPattern getSubpattern(int start, int length) const;

private:
    int m_length;
    int m_multiCharMap[MAX_NR_POSITIONS];
    int m_nrTrueBits[MAX_NR_POSITIONS];
};

#endif
