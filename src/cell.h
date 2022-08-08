/*
 *  cell.h
 *  combus
 *
 *  Created by Adi Botea on 24/06/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

 #ifndef _CELL_H_
 #define _CELL_H_
 
struct Cell {
    Cell(int row, int col)
    :m_row(row), m_col(col)
    {}
    Cell():m_row(-1), m_col(-1) {}
    virtual ~Cell() {}
    int m_row;
    int m_col;
    virtual int operator==(const Cell & rhs) const;
};

#endif
