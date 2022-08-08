/*
 *  multicharmap.cpp
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "multicharmap.h"
#include "wordslot.h"

MultiCharMap::MultiCharMap() {
    assert(sizeof (int) >= 4);
    for (int row = 0; row < MAX_NR_ROWS; row++) {
        for (int col = 0; col < MAX_NR_COLS; col++) {
            m_nrFalseBits[row][col] = 0;
            this->m_map[row][col][HORIZONTAL] = 0;
            this->m_map[row][col][VERTICAL] = 0;
            for (int c = 0; c < 'Z' - 'A' + 1; c++) {
                setBit(row, col, HORIZONTAL, c, true);
                setBit(row, col, VERTICAL, c, true);
            }
        }
    }
}

bool MultiCharMap::containsConflict() const {
    for (int row = 0; row < m_nrRows; row++) {
        for (int col = 0; col < m_nrCols; col++) {
            bool conflict = true;
            for (int c = 0; c < 'Z' - 'A' + 1; c++) {
                if (getBit(row, col, HORIZONTAL, c) &&
                        getBit(row, col, VERTICAL, c)) {
                    conflict = false;
                    break;
                }
            }
            if (conflict) {
                //cerr << "Conflict at position (" << row << " , " << col << " )" << endl;
                return true;
            }
        }
    }
    //cerr << "No conflict detected" << endl;
    return false;
}

bool MultiCharMap::getBit(int row, int col, int direction, int pos) const {
    assert(0 <= pos && pos <= 'Z' - 'A');
    int mask = 1 << (pos);
    return (m_map[row][col][direction] & mask) ? true : false;
}

void MultiCharMap::setBit(int row, int col, int direction, int pos, bool value) {
    assert(0 <= pos && pos <= 'Z' - 'A');
    bool oldValue = getBit(row, col, direction, pos);
    if (oldValue != value) {
        int mask = (1 << pos);
        m_map[row][col][direction] ^= mask;
    }
}

/*
 The input is a word slot.                 
 This method updates the m_map data structure with info about
 what characters are allowed in each slot cell according to the
 constraints of the pattern.
 */
void MultiCharMap::updateFromSlot(const WordSlot & slot) {
    int rowIdx, colIdx;
    for (int it = 0; it < slot.getLength(); it++) {
        if (slot.getSimplePattern()[it] != BLANK)
            continue;
        if (slot.getDirection() == HORIZONTAL) {
            rowIdx = slot.getRow();
            colIdx = slot.getCol() + it;
        } else {
            rowIdx = slot.getRow() + it;
            colIdx = slot.getCol();
        }
        int oldFalseBits = m_nrFalseBits[rowIdx][colIdx];
        m_nrFalseBits[rowIdx][colIdx] = 0;
        for (int letter = 'A'; letter <= 'Z'; letter++) {
            bool val = getBit(rowIdx, colIdx, slot.getDirection(), letter - 'A') &&
                    slot.getMultiCharValue(it, letter - 'A');
            setBit(rowIdx, colIdx, slot.getDirection(), letter - 'A', val);
            if ((get(rowIdx, colIdx, letter - 'A') == false)) {
                m_nrFalseBits[rowIdx][colIdx]++;
                //cerr << "Ruled out char " << letter << " in position ";
                //cerr << rowIdx << " , " << colIdx << endl;
            }
        }
        if (oldFalseBits != m_nrFalseBits[rowIdx][colIdx]) {
            assert(oldFalseBits <= m_nrFalseBits[rowIdx][colIdx]);
            assert(slot.getLetter(it) == BLANK);
            m_recentlyConstrainedCells.push_back(Cell(rowIdx, colIdx));
        }
    }
}

unsigned int MultiCharMap::m_nrRows;
unsigned int MultiCharMap::m_nrCols;
