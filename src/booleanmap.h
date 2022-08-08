/*
 *  booleanmap.h
 *  combus
 *
 *  Created by Adi Botea on 14/11/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BOOLEAN_MAP_H_
#define _BOOLEAN_MAP_H_

#include <vector>
#include <cassert>
#include "cellrecord.h"
#include "cell.h"
#include "constants.h"

using namespace std;

/*
 * As the name suggests, this implements a 
 * 2D array of boolean values, mirroring the grid.
 */
struct BooleanMap {
    BooleanMap();
    int getNrTrueValues() const {
        return m_nrTrueValues;
    };
    void markCells(const vector<Cell> & cells, bool value);
    void markCells(const vector<CellRecord> & cells, bool value);
    void setValue(int row, int col, bool value);
    bool getValue(int row, int col) const {
    	assert (0 <= row && row < MAX_NR_ROWS);
    	assert (0 <= col && col < MAX_NR_COLS);
    	return this->m_map[row][col];
    }
private:
    int m_nrTrueValues;
    bool m_map[MAX_NR_ROWS][MAX_NR_COLS];
};

#endif
