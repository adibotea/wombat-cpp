/*
 *  cell.cpp
 *  combus
 *
 *  Created by Adi Botea on 24/06/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "cell.h"

int Cell::operator==(const Cell & rhs) const {
    if (this->m_row != rhs.m_row || this->m_col != rhs.m_col)
        return 0;
    return 1;
}
