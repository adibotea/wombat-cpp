/*
 *  staticpuzleinfo.h
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STATICPUZZLEINFO_H_
#define _STATICPUZZLEINFO_H_

#include "dictionary.h"
#include <vector>
#include <iostream>

/*
 * As the name suggests, this class stores static info
 * such as the dictionaries and the size of the grid.
 */
class StaticPuzzleInfo {
public:
    StaticPuzzleInfo(int nrRows, int nrCols, bool allowBlackPoints,
            const std::vector<Dictionary> & horizDics,
            const std::vector<Dictionary> & vertDics);
    int m_nrRows;
    int m_nrCols;
    bool m_allowNewBlackPoints;
    std::vector<Dictionary> m_horizDics;
    std::vector<Dictionary> m_vertDics;
};

#endif
