/*
 *  staticpuzleinfo.cpp
 *  combus
 *
 *  Created by Adi Botea on 23/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "staticpuzzleinfo.h"


StaticPuzzleInfo::StaticPuzzleInfo(int nrRows, int nrCols, bool newBlackPoints,
                                   const std::vector<Dictionary> & horizDics,
                                   const std::vector<Dictionary> & vertDics)
:m_nrRows(nrRows), m_nrCols(nrCols), m_allowNewBlackPoints(newBlackPoints),
m_horizDics(horizDics), m_vertDics(vertDics)
{
}
