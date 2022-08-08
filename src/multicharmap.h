/*
 *  multicharmap.h
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MULTICHARMAP_H_
#define _MULTICHARMAP_H_

#include "cell.h"
#include <vector>
#include "constants.h"

using namespace std;

class WordSlot;

/*
 * A multi-char map is a multi-dimensional array as follows.
 * Given a row, a col and a direction (horiz or vert),
 * we store all characters not ruled out for the cell and direction at hand.
 */
class MultiCharMap {
public:
    MultiCharMap();
    ~MultiCharMap(){};
    void clearRecentlyConstrainedCells() {
        m_recentlyConstrainedCells.clear();
    };
    bool containsConflict() const;
    void updateFromSlot(const WordSlot & slot);
    static void setStaticInfo(unsigned int nrRows, unsigned int nrCols) {
        m_nrRows = nrRows;
        m_nrCols = nrCols;
    };
    bool get(unsigned int row, unsigned int col, int charIdx) const {
        return (getBit(row, col, HORIZONTAL, charIdx) &&
                getBit(row, col, VERTICAL, charIdx));
    };
    int getNrFalseBits(int row, int col) {
        return m_nrFalseBits[row][col];
    };
    vector<Cell> getRecentlyConstrainedCells() const {
        return m_recentlyConstrainedCells;
    };
private:
    static unsigned int m_nrRows;
    static unsigned int m_nrCols;
    // each integer is used as a bitmap -- one bit for each alphabet character
    int m_map[MAX_NR_ROWS][MAX_NR_COLS][2];
    int m_nrFalseBits[MAX_NR_ROWS][MAX_NR_COLS];
    vector<Cell> m_recentlyConstrainedCells;
    bool getBit(int row, int col, int direction, int pos) const;
    void setBit(int row, int col, int direction, int pos, bool value);

};

#endif
