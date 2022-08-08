/*
 *  CellRecord.cpp
 *  combus
 *
 *  Created by Adi Botea on 1/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "cellrecord.h"

int CellRecord::operator==(const CellRecord & rhs) const
{
    if (this->m_col != rhs.m_col || 
        this->m_row != rhs.m_row ||
        this->m_letter != rhs.m_letter) 
        return 0;
    else 
        return 1;
}
