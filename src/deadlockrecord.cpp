/*
 *  deadlockrecord.cpp
 *  combus
 *
 *  Created by Adi Botea on 3/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "deadlockrecord.h"
#include <algorithm>


DeadlockRecord DeadlockRecord::difference(const vector<Cell> & cellsToRemove) const {
    DeadlockRecord result;
    for (int it = 0; it < m_cells.size(); it++) {
        Cell cell(m_cells[it].m_row, m_cells[it].m_col);
        if (find(cellsToRemove.begin(), cellsToRemove.end(), cell) == 
            cellsToRemove.end())
            result.addCellRecord((const CellRecord & ) m_cells[it]);
    }
    return result;
}

/*
 * This implementation assumes that deadlock records have their cells
 * in a canonical order which starts from the upper left corner of the grid.
 */
int DeadlockRecord::operator==(const DeadlockRecord & record) const {
    if (this->getCells().size() != record.getCells().size())
        return 0;
    for (int it = 0; it < this->getCells().size(); it++) {
        if (!(this->getCells()[it] == record.getCells()[it]))
            return 0;
    }
    return 1;
}

void DeadlockRecord::setStaticInfo(unsigned int nrRows, unsigned int nrCols) {
    DeadlockRecord::m_nrRows = nrRows;
    DeadlockRecord::m_nrCols = nrCols;
}

unsigned int DeadlockRecord::m_nrRows;
unsigned int DeadlockRecord::m_nrCols;
