/*
 *  CellRecord.h
 *  combus
 *
 *  Created by Adi Botea on 1/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CELLRECORD_H_
#define _CELLRECORD_H_

#include "cell.h"

struct CellRecord : public Cell {
public:
    CellRecord(int row, int col, int letter)
    :Cell(row, col), m_letter(letter)
    {};
    CellRecord():Cell(-1, -1), m_letter('A')
    {};
    char m_letter;
    int operator==(const CellRecord &rhs) const;
};

#endif
