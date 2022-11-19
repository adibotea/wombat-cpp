/*
 *  state.cpp
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "state.h"
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <math.h>
#include <float.h>
#include <unordered_map>
#include "cellrecord.h"
#include <list>
#include <algorithm>
#include <string.h>
#include "limits.h"
#include "constants.h"
#include "globals.h"
#include "util.h"

using namespace std;
using namespace __gnu_cxx;

State::State() : m_propagationIterations(0), m_nrBlackPoints(0),
		m_nrThematicPoints(0), m_wordRepetition(false),
		m_complete(false), m_procFromScratch(true),
		m_skipDeadlockExtraction(false),
		m_0HitSlot(false), m_adjBPs(false), m_nrBPBlocks(0), m_goal(false) {
    this->m_nrFilledCells = 0;
    m_recentCellInstantiations.clear();
    for (int i = 0; i <= State::m_nrRows; i++) {
        for (int j = 0; j <= State::m_nrCols; j++) {
            m_grid[i][j] = BLANK;
        }
    }
    for (int i = 0; i < State::m_nrRows; i++) {
        for (int j = 0; j < State::m_nrCols; j++) {
            if (m_initGrid[i][j] != BLANK && !isBlackPoint(m_initGrid[i][j])) {
                m_recentCellInstantiations.push_back(Cell(i, j));
            }
            if (isBlackPoint(m_initGrid[i][j]) && !isBlackPoint(this->m_grid[i][j])) {
                this->addBlackPoint(i, j);
            } else {
                m_grid[i][j] = m_initGrid[i][j];
                if (m_grid[i][j] != BLANK)
                    m_nrFilledCells++;
            }
        }
    }
    this->m_recentBlackPointCol = -1;
    this->m_recentBlackPointRow = -1;
    collectSlots();
    for (int i = 0; i < this->m_slots.size(); i++) {
        this->m_slots[i].setAllMultiChar(true);
    }
    for (int i = 0; i < MAX_WORD_LENGTH; i++)
        this->m_nrNTW[i] = 0;
    this->m_idxSelSlotGen = -1;
    this->m_f = this->m_f2 = 0;
}

State::State(int iters, int start_slot) : m_propagationIterations(iters), m_nrBlackPoints(0), m_nrThematicPoints(
0),
m_nrFilledCells(0), m_wordRepetition(false), m_complete(
false),
m_procFromScratch(true), m_skipDeadlockExtraction(
false),
m_0HitSlot(false), m_adjBPs(false), m_nrBPBlocks(0) {
    this->m_idxSelSlotGen = start_slot;
    for (int i = 0; i <= State::m_nrRows; i++) {
        for (int j = 0; j <= State::m_nrCols; j++) {
            m_grid[i][j] = BLANK;
        }
    }
    for (int i = 0; i < State::m_nrRows; i++) {
        for (int j = 0; j < State::m_nrCols; j++) {
            if (m_initGrid[i][j] != BLANK && !isBlackPoint(m_initGrid[i][j]))
                m_recentCellInstantiations.push_back(Cell(i, j));
            if (isBlackPoint(m_initGrid[i][j])) {
                this->addBlackPoint(i, j);
            } else {
                m_grid[i][j] = m_initGrid[i][j];
                if (m_grid[i][j] != BLANK)
                    m_nrFilledCells++;
            }
        }
    }
    collectSlots();
    for (int i = 0; i < this->m_slots.size(); i++) {
        this->m_slots[i].setAllMultiChar(true);
    }
    for (int i = 0; i < MAX_WORD_LENGTH; i++)
        this->m_nrNTW[i] = 0;
    this->m_f = this->m_f2 = 0;
}

bool State::backjumpHere(const State &deadlockState) const {
    const vector<WordSlot> &slots = deadlockState.getDeadlockSlots();
    for (auto it = slots.begin(); it != slots.end();
            it++) {
        if (it->getDirection() != m_slots[m_idxSelSlotGen].getDirection())
            continue;
        if (it->getCol() != m_slots[m_idxSelSlotGen].getCol())
            continue;
        if (it->getRow() != m_slots[m_idxSelSlotGen].getRow())
            continue;
        return true;
    }
    return false;
}

void State::collectSlots() {
    m_slots.clear();
    for (int row = 0; row < m_nrRows; row++) {
        collectSlotsHoriz(row);
    }
    for (int col = 0; col < m_nrCols; col++) {
        collectSlotsVert(col);
    }
    for (auto it = m_slots.begin(); it != m_slots.end();
            it++) {
        it->computeImpactAreaSize(m_slots);
    }
}

void State::updateSlots() {
    for (auto & slot : m_slots) {
        string pattern;
        assert(pattern.size() == 0);
        for (int idx = 0; idx < slot.getLength(); idx++) {
            pattern += m_grid[slot.getCellRow(idx)][slot.getCellCol(idx)];
        }
        slot.setPattern(pattern);
    }
}

void State::collectSlotsHoriz(int row) {
    string pattern;
    assert(pattern.size() == 0);
    int col = 0, colStart = 0;
    while (col <= m_nrCols) {
        if (col == m_nrCols || isBlackPoint(m_grid[row][col])) {
            if (pattern.size() > 0) {
                assert(pattern.size() <= State::m_nrCols);
                for (int i = 0; i < pattern.size(); i++)
                    assert(!isBlackPoint(pattern[i]));
                WordSlot slot = WordSlot(row, colStart, HORIZONTAL, pattern,
                        m_slots.size());
                if (g_pm.getCorner() == 11) {
                    if (this->slotEndsWithOrigBP(slot))
                        m_slots.push_back(slot);
                } else {
                    m_slots.push_back(slot);
                }
            }
            pattern.clear();
            colStart = col + 1;
        } else {
            pattern += m_grid[row][col];
        }
        col++;
    }
}

void State::collectSlotsVert(int col) {
    string pattern;
    assert(pattern.size() == 0);
    int row = 0, rowStart = 0;
    while (row <= m_nrRows) {
        if (row == m_nrRows || isBlackPoint(m_grid[row][col])) {
            if (pattern.size() > 0) {
                assert(pattern.size() <= State::m_nrRows);
                WordSlot slot = WordSlot(rowStart, col, VERTICAL, pattern,
                        m_slots.size());
                if (g_pm.getCorner() == 11) {
                    if (this->slotEndsWithOrigBP(slot))
                        m_slots.push_back(slot);
                } else {
                    m_slots.push_back(slot);
                }
            }
            pattern.clear();
            rowStart = row + 1;
        } else {
            pattern += m_grid[row][col];
        }
        row++;
    }
}

void State::addLeftTopBlackpoints(int slot_idx) const {
    if (slot_idx == 0)
        State::m_conflictDetection = 0;
    if (slot_idx >= m_slots.size()) {
        string filename = "topleft-" + std::to_string(m_conflictDetection) + ".pzl";
        this->writeToFilePzl(filename);
        m_conflictDetection++;
        // cerr << "nr generated states: " << m_conflictDetection << endl;
    } else {
        int ref_length = 5;
        const WordSlot & slot = this->m_slots[slot_idx];
        //cerr << "slot number " << slot_idx << endl;
        bool slot_has_bp = false;
        if (slot.getDirection() == VERTICAL) {
            for (int r = 0; r < slot.getLength(); r++)
                if (m_grid[r][slot.getCol()] == BLACKPOINT)
                    slot_has_bp = true;
        }
        if (slot_has_bp) {
            State state = *this;
            state.addLeftTopBlackpoints(slot_idx + 1);
            return;
        }
        if (slot.getLength() >= ref_length + 1) {
            //cerr << "long slot" << endl;
            for (int xx = 1; xx <= 3; xx++) {
                State state = *this;
                int row = slot.getRow();
                int col = slot.getCol();
                if (slot.getDirection() == HORIZONTAL) {
                    col += slot.getLength() - ref_length - xx;
                    if (col < 0)
                        continue;
                } else {
                    row += slot.getLength() - ref_length - xx;
                    if (row < 0)
                        continue;
                }
                if (state.isBPLegal(row, col)) {
                    state.addBlackPoint(row, col);
                    state.addLeftTopBlackpoints(slot_idx + 1);
                }
            }
        } else {
            //cerr << "short slot" << endl;
            State state = *this;
            state.addLeftTopBlackpoints(slot_idx + 1);
        }
    }
}


// This method is used when a deadlock is detected.
// Starting from the slot with 0 successors,
// we gradually enlarge the area using slots
// that are not fully instantiated.
// The method returns the set of fully instantiated
// slots that intersect this area.

void State::computeDeadlockInfo(int seedSlotIdx, BooleanMap &boolMap) {
    if (seedSlotIdx == -1)
        return;
    //    bool boolMapInitiallyEmpty = (boolMap.getNrTrueValues() == 0);
    int iter = 0;
    vector<int> area;
    list<int> newlyAddedSlots;
    unordered_map<int, int> deadlockSlotIndexes;
    m_deadlockSlots.clear();
    newlyAddedSlots.push_front(seedSlotIdx);
    while (newlyAddedSlots.size() > 0) {
        iter++;
        int slotIdx = newlyAddedSlots.back();
        newlyAddedSlots.pop_back();
        boolMap.markCells(m_slots[slotIdx].getCells(), false);
        if (find(area.begin(), area.end(), slotIdx) == area.end()) {
            area.push_back(slotIdx);
            //            boolMap.markCells(m_slots[slotIdx].getCells(), false);
        }
        vector<int> slots = getIntersectingSlots(slotIdx);
        for (auto it = slots.begin(); it != slots.end();
                it++) {
            if (m_slots[*it].getPattern().isInstantiated()) {
                deadlockSlotIndexes[*it] = *it;
            } else {
                if ((iter > 6) && m_slots[*it].getPattern().isBlank() && m_slots[*it].getPattern().hasNoConstraints())
                    continue;
                if (find(area.begin(), area.end(), *it) != area.end())
                    continue;
                if (find(newlyAddedSlots.begin(), newlyAddedSlots.end(), *it) != newlyAddedSlots.end())
                    continue;
                newlyAddedSlots.push_front(*it);
            }
        }
    }
    for (auto it = deadlockSlotIndexes.begin();
            it != deadlockSlotIndexes.end(); it++)
        m_deadlockSlots.push_back(m_slots[it->second]);
    computeDeadlockCells(area);
}

void State::computeDeadlockCells(const vector<int> & slots) {
    unordered_map<int, CellRecord> cells;
    for (auto it = slots.begin(); it != slots.end(); it++) {
        const WordSlot & slot = m_slots[*it];
        for (int pos = 0; pos < slot.getLength(); pos++) {
            if (slot.getLetter(pos) != BLANK) {
                int index = slot.getCellRow(pos)*m_nrCols + slot.getCellCol(pos);
                cells[index] = CellRecord(slot.getCellRow(pos), slot.getCellCol(pos),
                                          slot.getSimplePattern()[pos]);
            }
        }
    }
    for (auto it = cells.begin(); it != cells.end(); it++) {
        assert (it->second.m_letter != BLANK);
        m_deadlock.addCellRecord(it->second);
    }
    m_deadlock.addNumbers(this->m_nrNTW);
}

void State::computeDeadlockInfo(BooleanMap &boolMap) {
    computeDeadlockInfo(m_idxSelSlotGen, boolMap);
}

double State::computeSlotScore(int it) const {
    return m_slots[it].getStaticScore();
    if (!g_pm.getSlotStatsFlag())
        return m_slots[it].getStaticScore();
    if (m_slots[it].getStaticScore() <= 1)
        return m_slots[it].getStaticScore();
    return m_slots[it].getStaticScore();
}

vector<int> State::getIntersectingSlots(int slotIdx) const {
    vector<int> result;
    for (int it = 0; it < m_slots.size(); it++) {
        if (it == slotIdx)
            continue;
        if (m_slots[slotIdx].slotsCrossEachOther(m_slots[it]))
            result.push_back(it);
    }
    return result;
}

string State::selPatAsString() const {
    char tmp[200];
    assert(m_idxSelSlotGen == -1 || m_idxSelSlotGen < m_slots.size());
    if (m_idxSelSlotGen == -1)
        return "<>";
    string str = "<" + m_slots[m_idxSelSlotGen].getSimplePattern() + ">";
    for (int it = 0; it < str.length(); it++)
        if (str[it] == BLANK)
            str[it] = '.';
    int ubbf = m_slots[m_idxSelSlotGen].getNrMatchingWords();
    if (!m_slots[m_idxSelSlotGen].getSterileFlag())
    	ubbf++;
    assert(m_slots[m_idxSelSlotGen].getCol() <= m_nrCols);
    assert(m_slots[m_idxSelSlotGen].getRow() <= m_nrRows);
    sprintf(tmp, " [row %d, col %d] -- [upper bound branch fact %d]",
            m_slots[m_idxSelSlotGen].getRow(),
            m_slots[m_idxSelSlotGen].getCol(),
            ubbf);
    assert(strlen(tmp) < 200);
    str += tmp;
    return str;
}

int State::countMatchingWords(const WordSlot &slot) const {
    int result = 0;
    assert(
            slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
    vector<Dictionary> &dics =
            (slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    for (auto it1 = (dics).begin();
            it1 != (dics).end(); it1++) {
        result += it1->getNrWordsMatchingPattern(slot.getPattern());
    }
    return result;
}

bool State::hasMatchingWord(const WordSlot &slot) const {
    if (slot.getLength() <= 2)
        return true;
    vector<Dictionary> &dics =
            (slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    for (auto it1 = dics.begin();
            it1 != dics.end(); it1++) {
        if (this->m_nrSterileWords >= g_pm.getMaxNrNonThemWords() && slot.getLength() >= MIN_LENGTH_NTW && !it1->isThematic())
            continue;
        if (it1->atLeast1Hit(slot.getPattern())) {
            return true;
        }
    }
    return false;
}

bool State::matchesCellPattern(const vector<CellRecord> &deadlock) const {
    for (auto it = deadlock.begin(); it != deadlock.end(); it++) {
        if (this->m_grid[it->m_row][it->m_col] != it->m_letter)
            return false;
    }
    return true;
}

bool State::wordIsThematic(const WordSlot &slot) const {
    assert(
            slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
    vector<Dictionary> &dics =
            (slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    for (auto it1 = (dics).begin(); it1 != (dics).end(); it1++) {
        if (!it1->isThematic())
            continue;
        if (it1->getNrWordsMatchingPattern(slot.getPattern()) == 1)
            return true;
    }
    return false;
}

/*
 Iterate through all slots affected by the last move
 and compute the multi-char map.
 */
void State::updateMultiCharMap() {
    for (int it = 0; it < m_slots.size(); it++) {
        if (m_slots[it].getLength() <= 2)
            continue;
        if (!slotContainsRecentlyUpdatedCell(m_slots[it], INSTANTIATION))
            continue;
        vector<Dictionary> &dics =
                (m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
        m_slots[it].updateMultiCharPatternFromGridMap(m_multiCharMap);
        m_slots[it].updateMultiCharPatternFromDictionaries(dics);
        m_multiCharMap.updateFromSlot(m_slots[it]);
    }
}

void State::computeRecentlyConstrainedCells(
        const int m1[MAX_NR_ROWS][MAX_NR_COLS],
        const int m2[MAX_NR_ROWS][MAX_NR_COLS]) {
    m_recentCellConstraints.clear();
    for (int row = 0; row < m_nrRows; row++) {
        for (int col = 0; col < m_nrCols; col++) {
            if (m1[row][col] != m2[row][col])
                m_recentCellConstraints.push_back(Cell(row, col));
        }
    }
}

int State::countBlanksAdjToLetters() const {
    int result = 0;
    for (int row = 0; row < m_nrRows; row++) {
        for (int col = 0; col < m_nrCols; col++) {
            if (m_grid[row][col] >= 'A' && m_grid[row][col] <= 'Z') {
                if (row > 0 && m_grid[row - 1][col] == BLANK)
                    result++;
                if (col > 0 && m_grid[row][col - 1] == BLANK)
                    result++;
                if (row < m_nrRows - 1 && m_grid[row + 1][col] == BLANK)
                    result++;
                if (col < m_nrCols - 1 && m_grid[row][col + 1] == BLANK)
                    result++;
            }
        }
    }
    return result;
}

double State::weightedSumLetters(int slot_idx) const {
    double result = 0;
    double ratio = 1.0;
    for (int cell_idx = 0; cell_idx < m_slots[slot_idx].getLength();
            cell_idx++) {
        int row = m_slots[slot_idx].getCellRow(cell_idx);
        int col = m_slots[slot_idx].getCellCol(cell_idx);
        if (m_grid[row][col] != BLANK)
            result += ratio;
        ratio *= .9;
    }
    return result;
}

int State::countBlanksAdjToSlot(int slot_idx) const {
    int result = 0;
    for (int cell_idx = 0; cell_idx < m_slots[slot_idx].getLength();
            cell_idx++) {
        int row = m_slots[slot_idx].getCellRow(cell_idx);
        int col = m_slots[slot_idx].getCellCol(cell_idx);
        if (row > 0 && m_grid[row - 1][col] == BLANK && m_slots[slot_idx].getDirection() == HORIZONTAL)
            result++;
        if (col > 0 && m_grid[row][col - 1] == BLANK && m_slots[slot_idx].getDirection() == VERTICAL)
            result++;
        if (row < m_nrRows - 1 && m_grid[row + 1][col] == BLANK && m_slots[slot_idx].getDirection() == HORIZONTAL)
            result++;
        if (col < m_nrCols - 1 && m_grid[row][col + 1] == BLANK && m_slots[slot_idx].getDirection() == VERTICAL)
            result++;
    }
    return result;
}

void State::collectSlotStatistics() {
    m_0HitSlot = false;
    this->analyseInstantiatedSlots();
    this->analyseUninstantiatedSlots();
    // apply a search that is consistent in exploring one area
    WordSlot &slot = m_slots[m_idxSelSlotGen];
}

int State::computeMaxPatternLength() const {
    int result = 0;
    for (int it = 0; it < m_slots.size(); it++) {
        if (m_slots[it].getPattern().isInstantiated())
            continue;
        if (m_slots[it].getLength() > result)
            result = m_slots[it].getLength();
    }
    return result;
}

int State::getFirstHorizBlank(int row) const {
    for (int colIt = 0; colIt < m_nrCols; colIt++) {
        if (m_grid[row][colIt] == BLANK)
            return colIt;
    }
    return NOTHING;
}

int State::getFirstVertBlank(int col) const {
    for (int rowIt = 0; rowIt < m_nrRows; rowIt++) {
        if (m_grid[rowIt][col] == BLANK)
            return rowIt;
    }
    return NOTHING;
}

string State::getRow(int i) const {
    string res;
    for (int j = 0; j < State::m_nrCols; j++) {
        res += m_grid[i][j];
    }
    return res;
}

string State::getCol(int j) const {
    string res;
    for (int i = 0; i < State::m_nrRows; i++) {
        res += m_grid[i][j];
    }
    return res;
}

bool State::isGridComplete() const {
    for (int it = 0; it < m_nrRows; it++) {
        if (getFirstHorizBlank(it) != NOTHING)
            return false;
    }
    return true;
}

void State::makeHorizMove(int row, int col, string word) {
    if (col + word.length() > m_nrCols) {
        cerr << "Error: (" << row << ", " << col << ", " << word;
        cerr << ")" << std::endl
                << this;
    }
    //cerr << "Word to add: " << word << "[" << row << "," << col << "]" << endl;
    //cerr << "Slot in grid: ";
    //for (int i = 0; i < word.length(); i++)
    //    cerr << m_grid[row][col + i];
    //cerr << endl;

    assert(col + word.length() <= m_nrCols);
    for (int i = 0; i < word.length(); i++) {
        assert(
                m_grid[row][col + i] == BLANK || m_grid[row][col + i] == word[i]);
        if (m_grid[row][col + i] == BLANK) {
            m_grid[row][col + i] = word[i];
            m_recentCellInstantiations.push_back(Cell(row, col + i));
            this->m_nrFilledCells++;
        }
    }
    if (col + word.length() < m_nrCols && !isBlackPoint(m_grid[row][col + word.length()])) {
        this->addBlackPoint(row, col + word.length());
    }
    if (col - 1 >= 0 && !isBlackPoint(m_grid[row][col - 1])) {
        this->addBlackPoint(row, col - 1);
    }
}

vector<int> State::markCommittedCells() const {
    vector<int> result;
    result.resize(225);
    for (int c = 0; c < 15; c++)
    for (int r = 0; r < 15; r++)
        result[c+15*r] = 0;
    for (auto slot : this->m_slots) {
        if (!slot.getSterileFlag() && slot.getLength() >= 3)
            continue;
        for (int idx = 0; idx < slot.getLength(); idx++) {
            int col = slot.getCellCol(idx);
            int row = slot.getCellRow(idx);
            result[col+15*row]++;
        }
    }
    return result;
}

int State::countSterileCommittments() const {
    int result = 0;
    for (auto slot : this->m_slots)
        if (slot.getSterileFlag() && slot.getLength() >= 3)
            result += slot.getNrBlanks();
    return result;
}

void State::makeMoveSwapFlag(const WordSlot &slot) {
    m_slots[slot.getId()].setSterileFlag(true);
    std::ostringstream ss;
    ss << "placeholder-" << slot.getCol() << "-" << slot.getRow() << "-" << slot.getLength();
    this->setLastWord(ss.str());
    this->m_initGridFlag = false;
}

void State::makeMove(const WordSlot &slot, string word) {
    if (m_slots.size() > 0) {
        assert(slot.getId() >= 0 && slot.getId() < m_slots.size());
        m_slots[slot.getId()].setPattern(word);
    }
    string word2 = slot.getSterileFlag()?tolower(word):word;
    this->setLastWord(word2);
    makeMove(slot.getRow(), slot.getCol(), slot.getDirection(), word);
}

void State::makeMove(int row, int col, bool direction, string word) {
    m_recentCellInstantiations.clear();
    this->m_recentBlackPointCol = -1;
    this->m_recentBlackPointRow = -1;
    if (direction == HORIZONTAL) {
        makeHorizMove(row, col, word);
    } else {
        makeVertMove(row, col, word);
    }
    m_moves.push_back(Move(col, row, direction, word));
    if (m_wordsInGrid.find(word) != m_wordsInGrid.end()) {
        m_wordRepetition = true;
    }
    updateSlots();
    /*
    if (this->m_procFromScratch)
        collectSlots();
    else
        updateSlots();
     */
    this->m_initGridFlag = false;
    //    updateSlotsFromGrid();
    //performStaticAnalysis();
}

void State::makeVertMove(int row, int col, string word) {
    if (row + word.length() > m_nrRows) {
        cerr << "Error: (" << row << ", " << col << ", " << word;
        cerr << ")" << endl
                << this;
    }
    for (int i = 0; i < word.length(); i++) {
        assert(
                m_grid[row + i][col] == BLANK || m_grid[row + i][col] == word[i]);
        if (m_grid[row + i][col] == BLANK) {
            m_grid[row + i][col] = word[i];
            m_recentCellInstantiations.push_back(Cell(row + i, col));
            this->m_nrFilledCells++;
        }
    }
    if (row + word.length() < m_nrRows && !isBlackPoint(m_grid[row + word.length()][col])) {
        this->addBlackPoint(row + word.length(), col);
    }
    if (row - 1 >= 0 && !isBlackPoint(m_grid[row - 1][col])) {
        this->addBlackPoint(row - 1, col);
    }
}

void State::performStaticAnalysis() {
    // As a side effect, updateCharacterMap computes the number
    // of matching words for the slots that were affected by the
    // most recent move. For all other slots, that number is computed
    // in collectPatternStatistics(). This trick saves some computation time.
    double weight = g_pm.getHeurWeight();
    int maxScore = 2*(State::m_nrRows*State::m_nrCols - m_nrBlackPoints);
    m_sterileLoad = this->computeSterileLoad();
    maxScore -= m_sterileLoad;
    m_f = 1000 + 0*m_nr2PointCells + 1*m_nrThematicPoints - weight*m_sterileLoad - 0*m_nrZeroCells;
    m_sterileBudget = maxScore - g_pm.getMinNrThemPoints();
    propagateConstraints(30);
    collectSlotStatistics();
    m_f2 = m_nrThematicPoints + weight*m_heurScore;
    m_cellBudget = maxScore - this->getNrThematicPoints();
    if (this->m_idxSelSlotGen == -1) {
        this->m_goal = true;
    }
}

bool State::slotContainsRecentlyUpdatedCell(const WordSlot &slot,
        cellUpdateType updateType) const {
    const vector<Cell> &recentChanges =
            (updateType == INSTANTIATION) ? m_recentCellInstantiations : m_recentCellConstraints;
    for (vector<Cell>::const_iterator it = recentChanges.begin();
            it != recentChanges.end(); ++it) {
        if (slot.getDirection() == HORIZONTAL)
            if (it->m_row == slot.getRow() && it->m_col >= slot.getCol() && it->m_col < slot.getCol() + slot.getLength())
                return true;
        if (slot.getDirection() == VERTICAL)
            if (it->m_col == slot.getCol() && it->m_row >= slot.getRow() && it->m_row < slot.getRow() + slot.getLength())
                return true;
    }
    return false;
}

void State::printVariableInfo(ostream &os) const {
    int vars[50];
    int nrCons = 0;
    for (int it = 0; it < 50; it++)
        vars[it] = 0;
    for (int it = 0; it < m_slots.size(); it++) {
        const WordSlot &slot = m_slots[it];
        vars[slot.getLength()]++;
        nrCons += slot.getLength();
    }
    os << "Nr constraints: " << nrCons << endl;
    for (int it = 0; it < 50; it++) {
        if (vars[it] != 0) {
            os << "Slots of length " << it << " : " << vars[it] << endl;
        }
    }
}

void State::readDictionaries(ifstream &file) {
    string line;
    getline(file, line);
    int nrDics = atoi(line.c_str());
    int maxRowCol = (m_nrCols > m_nrRows) ? (m_nrCols) : (m_nrRows);
    for (int it = 0; it < nrDics; it++) {
        assert(!file.eof());
        getline(file, line);
        bool isThematic = (line == "1") ? true : false;
        assert(!file.eof());
        getline(file, line);
        Dictionary dictionary(line, maxRowCol, isThematic,
                g_pm.getDicPercentage());
        assert(!file.eof());
        getline(file, line);
        if (line == "1")
            m_horizDics.push_back(dictionary);
        assert(!file.eof());
        getline(file, line);
        if (line == "1")
            m_vertDics.push_back(dictionary);
    }
    assert (nrDics == 2);
    assert (m_horizDics[0].isThematic());
    assert (!m_horizDics[1].isThematic());
    assert (m_vertDics[0].isThematic());
    assert (!m_vertDics[1].isThematic());
}

void State::readHeader(ifstream &file) {
    string line;
    assert(!file.eof());
    getline(file, line);
    State::m_nrRows = atoi(line.c_str());
    assert(!file.eof());
    getline(file, line);
    State::m_nrCols = atoi(line.c_str());
    assert(!file.eof());
    getline(file, line);
    State::m_allowNewBlackPoints = atoi(line.c_str());
    assert(!file.eof());
    getline(file, line);
    State::m_allowTwoChars[HORIZONTAL] = atoi(line.c_str());
    assert(!file.eof());
    getline(file, line);
    State::m_allowTwoChars[VERTICAL] = atoi(line.c_str());
    MultiCharMap::setStaticInfo(State::m_nrRows, State::m_nrCols);
}

void State::readInitGrid(ifstream &file) {
    string line;
    for (int rowIt = 0; rowIt < m_nrRows; rowIt++) {
        assert(!file.eof());
        getline(file, line);
        for (int colIt = 0; colIt < line.size(); colIt += 2) {
            if (colIt >= 2 * MAX_NR_COLS)
                break;
            m_initGrid[rowIt][colIt / 2] = toupper(line[colIt]);
        }
    }
}

void State::readStaticInfo(const string &filename) {
    ifstream file(filename.c_str());
    if (file.is_open()) {
        readHeader(file);
        readInitGrid(file);
        readDictionaries(file);
        file.close();
    } else {
        cerr << "Cannot open file " << filename << endl;
    }
}

bool State::semiClosure() const {
    return false;
}

void State::updateSlotsFromGrid() {
    for (int it = 0; it < m_slots.size(); it++) {
        WordSlot &slot = m_slots[it];
        updateSlotFromGrid(slot);
    }
}

bool State::isBPLegal(int row, int col) const {
    if (this->m_grid[row][col] != BLANK)
        return false;
    if (row > 0 && isBlackPoint(this->m_grid[row - 1][col]))
        return false;
    if (row < this->m_nrRows && isBlackPoint(this->m_grid[row + 1][col]))
        return false;
    if (col > 0 && isBlackPoint(this->m_grid[row][col - 1]))
        return false;
    if (col < this->m_nrCols && isBlackPoint(this->m_grid[row][col + 1]))
        return false;
    return true;
}

bool State::isDeadlock() const {
    //if (m_branchingFactor == 0) {
    //    cerr << "Branching factor = 0" << endl;
    //    cerr << "Most constrained pattern: " << this->getMostConstraintPattern() << endl;
    //}
    string reason = "";
    bool result = false;
    if (false) { // && m_branchingFactor == 0) {
        reason = "BF == 0";
        result = true;
    }
    if (this->m_sterileBudget < 0) {
        reason = "them budget exceeded";
        result = true;
    }
    if (m_multiCharMap.containsConflict()) {
        reason = "conflict";
        result = true;
    }
    if (this->m_0HitSlot) {
        //cerr << "0-hit slot detected" << endl;
        //cerr << *this << endl << "---" << endl;
        reason = "0-hit slot";
        result = true;
    }
    int sid = m_idxSelSlotGen;
    if (this->m_initGridFlag && sid != -1) {
    	if (sid < 0 || sid >= m_slots.size() || m_slots[sid].getLength() < 3) {
            reason = "bad start slot";
            result = true;
        }
    }

    if (result && g_pm.getVerbosity() >= 3) {
        cerr << "The successor below is pruned away as deadlocked:" << endl;
        cerr << "Reason: " << reason << endl;
        cerr << *this;
        cerr << endl;
    }
    return result;
}

vector<int> State::getLegalBPPos(const WordSlot &slot) const {
    int bpc = slot.getCol();
    int bpr = slot.getRow();
    vector<int> result;
#ifdef NO_BPS
    return result;
#endif
    for (unsigned int len = 0; len < slot.getLength(); len++) {
        if (slot.getDirection() == HORIZONTAL)
            bpc = slot.getCol() + len;
        else
            bpr = slot.getRow() + len;
        if (this->isBPLegal(bpr, bpc))
            result.push_back(len);
    }
    return result;
}

void State::updateSlotFromGrid(WordSlot &slot) {
    string pattern = "";
    for (int pos = 0; pos < slot.getLength(); pos++) {
        pattern += m_grid[slot.getCellRow(pos)][slot.getCellCol(pos)];
    }
    slot.setPattern(pattern);
    slot.updateMultiCharPatternFromGridMap(m_multiCharMap);
}

int State::computeSterileLoad() {
    int cells[MAX_NR_COLS][MAX_NR_ROWS];
    int cells2[MAX_NR_COLS][MAX_NR_ROWS];
    this->m_nrZeroCells = 0;
    this->m_nr2PointCells = 0;
    for (int r = 0; r < MAX_NR_ROWS; r++)
        for (int c = 0; c < MAX_NR_COLS; c++) {
            cells[c][r] = 0;
            cells2[c][r] = 0;
        }
    int result = 0;
    for (int it = 0; it < m_slots.size(); it++) {
        if (m_slots[it].getLength() < 3) {
            result += m_slots[it].getLength();
            for (int idx = 0; idx < m_slots[it].getLength(); idx++)
                cells[m_slots[it].getCellCol(idx)][m_slots[it].getCellRow(idx)]++;
            continue;
        }
        if (!m_slots[it].getPattern().isInstantiated()) {
            if (m_slots[it].getSterileFlag()) {
                result += m_slots[it].getLength();
                for (int idx = 0; idx < m_slots[it].getLength(); idx++)
                    cells[m_slots[it].getCellCol(idx)][m_slots[it].getCellRow(idx)]++;
            } else 
                for (int idx = 0; idx < m_slots[it].getLength(); idx++)
                    cells2[m_slots[it].getCellCol(idx)][m_slots[it].getCellRow(idx)]++;
            continue;
        }
        if (!wordIsThematic(m_slots[it])) {
            result += m_slots[it].getLength();
            for (int idx = 0; idx < m_slots[it].getLength(); idx++)
                cells[m_slots[it].getCellCol(idx)][m_slots[it].getCellRow(idx)]++;
        }
    }
    for (int r = 0; r < MAX_NR_ROWS; r++)
        for (int c = 0; c < MAX_NR_COLS; c++) {
            if (cells[c][r] >= 2)
                this->m_nrZeroCells++;
            if (cells2[c][r] >= 2)
                this->m_nr2PointCells++;
        }
    return result;
}

void State::analyseInstantiatedSlots() {
    this->m_nrThematicPoints = 0;
    this->m_nrSterileWords = 0;
    this->m_nrThemSlots = 0;
    this->m_nrSterileSlots = 0;
    this->m_nrOpenSlots = 0;
    for (int kk = 0; kk < MAX_WORD_LENGTH; kk++)
        this->m_nrNTW[kk] = 0;
    m_wordsInGrid.clear();
    this->m_nrSlots.clear();
    for (int kk = 0; kk < 6; kk++)
        this->m_nrSlots.push_back(0);
    for (int it = 0; it < m_slots.size(); it++) {
        if (m_slots[it].getLength() < 3) {
            //	this->m_nrSlots[m_slots[it].getLength()]++;
            continue;
        }
        bool instantiated = m_slots[it].getPattern().isInstantiated();
        bool thematic = false;
        if (instantiated) {
            thematic = wordIsThematic(m_slots[it]);
            m_slots[it].setSterileFlag(!thematic);
        }
        if (m_slots[it].getSterileFlag())
            this->m_nrSterileSlots++;
        else if (instantiated)
            this->m_nrThemSlots++;
        else 
            this->m_nrOpenSlots++;
        if (!instantiated) {
            if (m_slots[it].getSterileFlag()) {
                if (m_slots[it].getLength() > MIN_LENGTH_NTW)
                    this->m_nrSterileWords++;
                assert(m_slots[it].getLength() < MAX_WORD_LENGTH);
                this->m_nrNTW[m_slots[it].getLength()]++;
            }
            continue;
        }
        if (m_slots[it].getLength() < 6) {
            if (!thematic)
                this->m_nrSlots[m_slots[it].getLength()]++;
        }
        m_slots[it].setNrMatchingWords(1);
        string word = m_slots[it].getSimplePattern();
        if (m_wordsInGrid.find(word) != m_wordsInGrid.end()) {
            m_wordRepetition = true;
            m_skipDeadlockExtraction = true;
        }
        m_wordsInGrid[word] = word;
        if (wordIsThematic(m_slots[it]))
            m_nrThematicPoints += word.size();
        else {
            if (word.size() > MIN_LENGTH_NTW)
                this->m_nrSterileWords++;
            assert(word.size() < MAX_WORD_LENGTH);
            this->m_nrNTW[word.size()]++;
        }
    }
}

bool State::isSterileOk(const WordSlot & slot) const {
    bool non_them_ok = (this->m_nrSterileWords < g_pm.getMaxNrNonThemWords());
    int len = slot.getLength();
    if (len > this->m_sterileBudget)
        non_them_ok = false;
    if (len == 3 && this->m_nrNTW[3] >= g_pm.getNTW3())
        non_them_ok = false;
    if (len == 4 && this->m_nrNTW[4] >= g_pm.getNTW4())
        non_them_ok = false;
    if (len == 5 && this->m_nrNTW[5] >= g_pm.getNTW5())
        non_them_ok = false;
    if (len == 6 && this->m_nrNTW[6] >= g_pm.getNTW6())
        non_them_ok = false;
    return non_them_ok;
}

bool State::shortStartSlot() const {
    int sid = m_idxSelSlotGen;
    if (this->m_initGridFlag && sid != -1) {
    	if (sid < 0 || sid >= m_slots.size() || m_slots[sid].getLength() < 3) {
			//cerr << "Slot of length 1 or 2 given with -start_slot option." << endl;
            return true;
		}
    }
    return false;
}

void State::addBlackPoint(int row, int col) {
    assert(0 <= row && row < m_nrRows && 0 <= col && col < m_nrCols);
    assert (!isBlackPoint(m_grid[row][col]));
    m_grid[row][col] = BLACKPOINT;
    m_recentBlackPointRow = row;
    m_recentBlackPointCol = col;
    this->m_nrBlackPoints++;
    this->m_nrFilledCells++;
    if (this->adjacentBlackPoints(row, col))
        this->m_adjBPs = true;
    m_recentCellInstantiations.push_back(Cell(row, col));
}

void State::addBlackPoint2(int row, int col) {
    assert(0 <= row && row < m_nrRows && 0 <= col && col < m_nrCols);
    assert (!isBlackPoint(m_grid[row][col]));
    m_grid[row][col] = BLACKPOINT2;
    m_recentBlackPointRow = row;
    m_recentBlackPointCol = col;
    this->m_nrBlackPoints++;
    this->m_nrFilledCells++;
    if (this->adjacentBlackPoints(row, col))
        this->m_adjBPs = true;
    m_recentCellInstantiations.push_back(Cell(row, col));
}

bool State::adjacentBlackPoints(int row, int col) const {
    assert (row >= 0 && col >= 0);
    bool common_edge = false;
    if (row > 0 && isBlackPoint(m_grid[row - 1][col]))
        common_edge = true;
    if (col > 0 && isBlackPoint(m_grid[row][col - 1]))
        common_edge = true;
    if (row < State::m_nrRows - 1)
        if (isBlackPoint(m_grid[row + 1][col]))
            common_edge = true;
    if (col < State::m_nrCols - 1)
        if (isBlackPoint(m_grid[row][col + 1]))
            common_edge = true;
    return common_edge;
};

bool State::slotEndsWithOrigBP(const WordSlot & slot) const {
    if (slot.getDirection() == HORIZONTAL) {
        if (slot.getCol() + slot.getLength() >= MAX_NR_COLS)
            return false;
        if (m_initGrid[slot.getRow()][slot.getCol() + slot.getLength()] == BLACKPOINT2)
            return true;
    } else {
        if (slot.getRow() + slot.getLength() >= MAX_NR_ROWS)
            return false;
        if (m_initGrid[slot.getRow() + slot.getLength()][slot.getCol()] == BLACKPOINT2)
            return true;
    }
    return false;
}

void State::analyseUninstantiatedSlots() {
    //cerr << *this << endl;
    int sid = m_idxSelSlotGen;
    if (this->m_initGridFlag && sid == -1) {
        int maxinfl = -1;
        for (int it = 0; it < m_slots.size(); it++) {
            WordSlot & slot = m_slots[it];
            if (slot.getLength() < 3)
                continue;
            if (slot.getPattern().isInstantiated())
                continue;
            int imp = slot.computeImpactAreaSize(m_slots);
            if (imp > maxinfl) {
                maxinfl = imp;
                m_idxSelSlotGen = it;
            }
        }
    }
    if (this->m_initGridFlag && m_idxSelSlotGen != -1) {
        return;
    }
    this->m_heurScore = 0;
    m_oldSlotIdx = this->m_idxSelSlotGen;
    this->m_idxSelSlotGen = -1;
    this->m_minSlotScore = DBL_MAX;
    for (int it = 0; it < m_slots.size(); it++) {
        const WordSlot & slot = m_slots[it];
        if (slot.getLength() < 3)
            continue;
        if (slot.getPattern().isInstantiated())
            continue;
        if (m_slots[it].getCol() < g_pm.getStartCol() ||
                m_slots[it].getCol() >= g_pm.getStartCol() + g_pm.getHorizLen())
            continue;
        if (m_slots[it].getRow() < g_pm.getStartRow() ||
                m_slots[it].getRow() >= g_pm.getStartRow() + g_pm.getVertLen())
            continue;
        if (!m_slots[it].compatWithCornerSel())
            continue;
        double slot_score = m_slots[it].getNrMatchingWords();
        if (false && slot_score <= 0 && !m_slots[it].getSterileFlag()) {
        //    m_nonThemBudget -= m_slots[it].getLength(); 
            m_slots[it].setSterileFlag(true);
            m_slots[it].setNrMatchingWords();
            slot_score = m_slots[it].getNrMatchingWords();
        }
        if (!m_slots[it].getSterileFlag())
            this->m_heurScore += m_slots[it].getLength();
        if (m_slots[it].getSterileFlag() && slot_score > 1)
            slot_score += 100;
        //slot_score += 1.0/(1.0*m_slots[it].getLength());
        //if (m_oldSlotIdx >= 0)
        //    if (!m_slots[it].slotsCrossEachOther(m_slots[m_oldSlotIdx]))
        //        slot_score += 20;
        if (m_slots[it].getLength() <= 3 && slot_score > 0)
            slot_score += 100;
        if (slot_score < this->m_minSlotScore) {
            m_idxSelSlotGen = it;
            this->m_minSlotScore = slot_score;
        }
    }
}

void State::propagateConstraints(int nrIterations) {
    unordered_map<int, int> oldUpdatedSlots, currentUpdatedSlots;
    int iter = 0, iter2 = 0;
    bool deadlock = false;
    m_multiCharMap.clearRecentlyConstrainedCells();
    if (false && m_initGridFlag) {
        // first get the domain (list of words) for each slot
        for (int it = 0; it < m_slots.size(); it++) {
            if (m_slots[it].getLength() <= 2)
                continue;
            bool nonthemok = this->isSterileOk(m_slots[it]);
            vector<Dictionary> & dics =
                    (m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
            m_slots[it].computeDomain(dics, nonthemok);
            currentUpdatedSlots[it] = it;
        }
    } else {
        // first get the domain (list of words) for each slot
        for (int it = 0; it < m_slots.size(); it++) {
            if (m_slots[it].getLength() <= 2)
                continue;
            //if (!slotContainsRecentlyUpdatedCell(m_slots[it], INSTANTIATION))
            //    continue;
            if (m_slots[it].getPattern().isInstantiated())
                continue;
            vector<Dictionary> & dics =
                    (m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
            m_slots[it].updateMultiCharPatternFromGridMap(m_multiCharMap);
            bool nonthemok = this->isSterileOk(m_slots[it]);
            m_slots[it].computeDomain(dics, nonthemok);
            m_slots[it].downwardsPropagation(dics);
            m_multiCharMap.updateFromSlot(m_slots[it]);
            currentUpdatedSlots[it] = it;
        }
    }
    // now start iterative propagation of channelling constraints
    while (true) {
        iter++;
        oldUpdatedSlots = currentUpdatedSlots;
        currentUpdatedSlots.clear();
        m_recentCellConstraints = m_multiCharMap.getRecentlyConstrainedCells();
        m_multiCharMap.clearRecentlyConstrainedCells();
        for (int it = 0; it < m_slots.size(); it++) {
            if (oldUpdatedSlots.find(it) != oldUpdatedSlots.end())
                continue;
            if (m_slots[it].getLength() <= 2)
                continue;
            if (!slotContainsRecentlyUpdatedCell(m_slots[it], CONSTRAINT))
                continue;
            if (m_slots[it].getPattern().isInstantiated())
                continue;
            iter2++;
            m_slots[it].updateMultiCharPatternFromGridMap(m_multiCharMap);
            vector<Dictionary> & dics =
                    (m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
            if (!m_slots[it].domainIsComputed()) {
                bool nonthemok = this->isSterileOk(m_slots[it]);
                m_slots[it].computeDomain(dics, nonthemok);
            } else {
                if (m_slots[it].getPattern().isBlank()) {
                    int posIdx = m_slots[it].getPattern().getMostConstrainedPos();
                    vector<int> chars = m_slots[it].getPattern().getTrueBits(posIdx);
                    if (chars.size() < 15)
                        m_slots[it].upwardsPropagation(dics);
                } else
                    m_slots[it].upwardsPropagation(dics);
            }
            // down propagation of blanks is expensive and likely not useful
            if (m_slots[it].getPattern().isBlank()) {
                int posIdx = m_slots[it].getPattern().getMostConstrainedPos();
                vector<int> chars = m_slots[it].getPattern().getTrueBits(posIdx);
                if (chars.size() < 15)
                    m_slots[it].downwardsPropagation(dics);
            } else {
                m_slots[it].downwardsPropagation(dics);
            }
            m_multiCharMap.updateFromSlot(m_slots[it]);
            if (m_slots[it].getNrMatchingWords() <= 0)
                deadlock = true;
            currentUpdatedSlots[it] = it;
        }
        if (iter == nrIterations) {
            //cerr << "max iterations: " << iter << endl;
            break;
        }
        if (m_recentCellConstraints.size() == 0) {
            //cerr << "fixpoint: " << iter << endl;
            break;
        }
        if (deadlock) {
            //cerr << "deadlock: " << iter << endl;
            break;
        }
    }
    //    cout << iter2 << endl;
    for (int it = 0; it < m_slots.size(); it++) {
        if (m_slots[it].getLength() <= 2)
            continue;
        m_slots[it].setNrMatchingWords();
        m_slots[it].clearDomain();
    }
    m_propIterationStats.addDataPoint(iter2);
}

unsigned int State::m_nrRows;
unsigned int State::m_nrCols;
vector<Dictionary> State::m_horizDics;
vector<Dictionary> State::m_vertDics;
bool State::m_allowNewBlackPoints;
bool State::m_allowTwoChars[2];
char State::m_initGrid[MAX_NR_ROWS][MAX_NR_COLS];
//unsigned int State::m_conflictDetection;
int State::m_propagationOrdering;
Statistics State::m_propIterationStats;
unsigned int State::m_conflictDetection;

