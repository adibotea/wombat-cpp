/*
 *  booleanmap.cpp
 *  combus
 *
 *  Created by Adi Botea on 14/11/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "booleanmap.h"
#include <assert.h>

BooleanMap::BooleanMap()
:m_nrTrueValues(0) 
{
    for (int it = 0; it < MAX_NR_ROWS; it++)
        for (int it2 = 0; it2 < MAX_NR_COLS; it2++)
            m_map[it][it2] = false;
}

void BooleanMap::setValue(int row, int col, bool value) {
    if (value == true && m_map[row][col] == false) {
        m_map[row][col] = true;
        m_nrTrueValues++;
    }
    if (value == false && m_map[row][col] == true) {
        m_map[row][col] = false;
        m_nrTrueValues--;
    }
    assert (m_nrTrueValues >= 0);
}

void BooleanMap::markCells(const vector<Cell> & cells, bool value) {
    for (auto it = cells.begin();
         it != cells.end(); it++) {
        setValue(it->m_row, it->m_col, value);
    }
}

void BooleanMap::markCells(const vector<CellRecord> & cells, bool value) {
    for (auto it = cells.begin();
         it != cells.end(); it++) {
        setValue(it->m_row, it->m_col, value);
    }
}
