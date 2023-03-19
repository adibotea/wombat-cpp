/*
 *  randomwritestate.cpp
 *  combus
 *
 *  Created by Adi Botea on 7/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <queue>
#include <algorithm>
#include "randomwritestate.h"
#include "wordslot.h"
#include "constants.h"
#include "parammanager.h"
#include "globals.h"
#include "util.h"

using namespace std;

RandomWriteState::RandomWriteState() :
State(), m_nrAllSuccessors(0), m_nrBPMoves(0) {
    this->m_initGridFlag = true;
    staticAnalAndTest(0, g_pm.getPruneWithBPs());
}

RandomWriteState::RandomWriteState(int iters, int start_slot) :
State(iters, start_slot), m_nrAllSuccessors(0), m_nrBPMoves(0) {
    this->m_initGridFlag = true;
    staticAnalAndTest(0, g_pm.getPruneWithBPs());
}

RandomWriteState::~RandomWriteState() {
}

vector<RandomWriteState> RandomWriteState::generateSuccessors(int bestSolScore,
        bool them_words, bool prune_with_bps) {
    vector<RandomWriteState> result;
    if (this->m_idxSelSlotGen == -1) {
        return result;
    } else {
        if (g_pm.getExpStyle() == 0 || this->m_initGridFlag)
            return this->genSuccsPartDelay(m_slots[this->m_idxSelSlotGen],
                    bestSolScore, them_words, prune_with_bps);
        else
            return this->genSuccsStd(m_slots[this->m_idxSelSlotGen],
                    bestSolScore, them_words, prune_with_bps);
    }
}

vector<RandomWriteState> RandomWriteState::genSuccsStd(const WordSlot & sel_slot, 
        int bestSolScore, bool them_words, bool prune_with_bps) {
    m_nrAllSuccessors = 0;
    int counter = 0;
    bool them = !sel_slot.getSterileFlag();
    vector<RandomWriteState> result;
    const WordPattern pat = sel_slot.getPattern();
    assert(
            sel_slot.getDirection() == HORIZONTAL || sel_slot.getDirection() == VERTICAL);
    vector<Dictionary> & dics =
            (sel_slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    this->m_skipDeadlockExtraction = false;
    int len = pat.getLength();
    WordPattern this_pat = pat;
    for (auto it1 = dics.begin(); it1 != dics.end(); it1++) {
        vector<string> words = it1->expandPattern(this_pat);
        for (auto it2 = words.begin(); it2 != words.end(); it2++) {
            counter++;
            if (g_pm.getMaxBranchingFactor() >= 0
                    && counter > g_pm.getMaxBranchingFactor())
                break;
            RandomWriteState state = *this;
            state.setPropagFromScratch(true);
            state.makeMove(sel_slot, *it2);
            if (!g_pm.getDelayedStaticAnal()) {
                if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
                    continue;
            }
            result.push_back(state);
            if (g_pm.getVerbosity() >= 3) {
                cerr << "ADDING SUCCESSOR " << endl;
                cerr << state << endl;
            }
        }
    }

    if (result.size() > 0) {
        if (g_pm.getVerbosity() >= 4) {
            cerr << "Before sorting" << endl;
            for (auto xx : result) {
                cerr << xx << endl;
            }
        }
        sort(result.begin(), result.end());
        reverse(result.begin(), result.end());
        if (g_pm.getVerbosity() >= 4) {
            cerr << "After reverting" << endl;
            for (auto xx : result) {
                cerr << xx << endl;
            }
        }
    }
    return result;
}

vector<RandomWriteState> RandomWriteState::genSuccsPartDelay(const WordSlot & sel_slot, 
        int bestSolScore, bool them_words, bool prune_with_bps) {
    //assert(!only_them_words);
    m_nrAllSuccessors = 0;
    int counter = 0;
    bool them = !sel_slot.getSterileFlag();
    vector<RandomWriteState> result;
    const WordPattern pat = sel_slot.getPattern();
    assert(
            sel_slot.getDirection() == HORIZONTAL || sel_slot.getDirection() == VERTICAL);
    vector<Dictionary> & dics =
            (sel_slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    this->m_skipDeadlockExtraction = false;
    int len = pat.getLength();
    WordPattern this_pat = pat;
    for (auto it1 = dics.begin(); it1 != dics.end(); it1++) {
        if (them && !it1->isThematic())
            continue;
        if (!them && it1->isThematic())
            continue;
        vector<string> words = it1->expandPattern(this_pat);
        for (auto it2 = words.begin(); it2 != words.end(); it2++) {
            counter++;
            if (g_pm.getMaxBranchingFactor() >= 0
                    && counter > g_pm.getMaxBranchingFactor())
                break;
            RandomWriteState state = *this;
            state.setPropagFromScratch(true);
            state.makeMove(sel_slot, *it2);
            if (!g_pm.getDelayedStaticAnal()) {
                if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
                    continue;
            }
            result.push_back(state);
            if (g_pm.getVerbosity() >= 3) {
                cerr << "ADDING SUCCESSOR " << endl;
                cerr << state << endl;
            }
        }
    }
    // Add the special successor with the thematic flag swapped to the sel slot
    if (them && this->isSterileOk(sel_slot)) {
        RandomWriteState state = *this;
        state.setPropagFromScratch(true);
        state.makeMoveSwapFlag(sel_slot);
        if (!g_pm.getDelayedStaticAnal()) {
            //cerr << "Running staticAnalAndTest" << endl;
            if (state.staticAnalAndTest(bestSolScore, prune_with_bps)) {
                result.push_back(state);
                if (g_pm.getVerbosity() >= 3) {
                    cerr << "ADDING SUCCESSOR WITH THEM FLAG SWAPPING" << endl;
                    cerr << state << endl;
            }

            }
        }
    }

    if (result.size() > 0) {
        if (g_pm.getVerbosity() >= 4) {
            cerr << "Before sorting" << endl;
            for (auto xx : result) {
                cerr << xx << endl;
            }
        }
        sort(result.begin(), result.end());
        reverse(result.begin(), result.end());
        if (g_pm.getVerbosity() >= 4) {
            cerr << "After reverting" << endl;
            for (auto xx : result) {
                cerr << xx << endl;
            }
        }
    }
    return result;
}

vector<RandomWriteState> RandomWriteState::genSuccsAL(const WordSlot & slot,
        int bestSolScore, bool only_them_words, bool prune_with_bps) {
    //assert(!only_them_words);
    m_nrAllSuccessors = 0;
    int counter = 0;
    vector<RandomWriteState> result;
    const WordPattern & pat = slot.getPattern();
    assert(
            slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
    vector<Dictionary> & dics =
            (slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
    this->m_skipDeadlockExtraction = false;
    // Generate words of variable lengths within this slot.
    // TODO: Currently inefficient because we don't check *early enough*
    // if some of these would introduce adjacent black points
    {
        WordPattern this_pat = pat;
        for (vector<Dictionary>::const_iterator it1 = dics.begin();
                it1 != dics.end(); it1++) {
            if (only_them_words && !it1->isThematic())
                continue;
            if (this->m_nrSterileWords >= g_pm.getMaxNrNonThemWords()
                    && pat.getLength() >= MIN_LENGTH_NTW && !it1->isThematic())
                continue;
            vector<string> words = it1->expandPattern(this_pat);
            for (vector<string>::iterator it2 = words.begin();
                    it2 != words.end(); it2++) {
                counter++;
                if (g_pm.getMaxBranchingFactor() >= 0
                        && counter > g_pm.getMaxBranchingFactor())
                    break;
                RandomWriteState state = *this;
                state.setPropagFromScratch(true);
                state.makeMove(slot, *it2);
                if (!g_pm.getDelayedStaticAnal())
                    if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
                        continue;
                result.push_back(state);
                if (g_pm.getVerbosity() >= 3) {
                    cerr << "ADDING SUCCESSOR " << endl;
                    cerr << state << endl;
                }
            }
        }
    }
    if (result.size() > 0) {
        sort(result.begin(), result.end());
        reverse(result.begin(), result.end());
    }
    return result;
}

bool RandomWriteState::staticAnalAndTest(double bestSolScore,
        bool prune_with_bps) {
    this->setPropagFromScratch(true);
    this->performStaticAnalysis();
    if (!this->isPromising())
        return false;
    if (!this->isLegal()) {
        return false;
    }
    if (this->isDeadlock()) {
        //cerr << "Successor deadlocked ( move ";
        //cerr << *it2 << " )" << endl;
        return false;
    }
    //if (this->cannotReachThemPoints(bestSolScore) && g_pm.getCorner() == 0)
    //    return false;
    //if (this->cannotReachThemPoints(g_pm.getMinThemPoints()) && g_pm.getCorner() == 0)
    //    return false;
    if (prune_with_bps && this->willExceedBPs())
        return false;
    return true;
}

bool RandomWriteState::isPromising() const {
    if (this->m_nrZeroCells > g_pm.getMaxZeroCells())
        return false;
    return true;
    int filled = this->getNrFilledCells() - this->m_nrBlackPoints + this->countSterileCommittments();
    bool cond2 = filled > 30 && 1.2*filled >= this->getNrThematicPoints();
    if (cond2) {
        //cerr << "Filled cells: " << filled << endl;
        //cerr << "Successor pruned away as not promising" << endl;
        //cerr << *this << endl;
        //exit(1);
        return false;
    }
    return true;
    double threshold = g_pm.getPayloadPruning();
    int pn = m_cellBudget - m_sterileBudget;
    double ratio = 1.0*m_sterileBudget/m_cellBudget;
    if (ratio < threshold && m_cellBudget > 100) {
        //cerr << "Successor pruned away as not promising" << endl;
        //cerr << *this << endl;
        //cerr << pn << " " << r << endl;
        //exit(1);
        return false;
    }
    return true;
    if (m_sterileBudget > 1 && 
        1.0*m_sterileBudget/m_cellBudget < .05) {
        //cerr << "Successor pruned away as not promising" << endl;
        //cerr << *this << endl;
        //exit(1);
        return false;
    }
	return true;
    bool result = true;
    if (this->m_nrThemSlots >= 2 && this->m_nrSterileSlots >= this->m_nrThemSlots)
        result = false;
    return result;
    int nrLetters = this->getNrFilledCells() - this->getNrBPs();
    if (nrLetters > 15) {
        bool condition = (getNrThematicPoints() < nrLetters - 5);
        if (condition) {
            result = false;
            cerr << "Successor pruned away as not promising" << endl;
            cerr << *this;
            cerr << endl;
        }
    }
    if (false && this->getNrFilledCells() > 60) {
        bool cond2 = 1.0 * this->getNrBPs() / this->getNrFilledCells() > 0.2;
        if (cond2) {
            result = false;
            cerr << "Successor pruned away as not promising" << endl;
            cerr << *this;
            cerr << endl;
        }
    }
    return result;
}

bool RandomWriteState::isConnected() const {
    return true;
    BooleanMap bm;
    queue<Cell> q;
    for (int col = 0; col < this->m_nrCols; col++) {
        if (!isBlackPoint(m_grid[0][col])) {
            q.push(Cell(0, col));
            bm.setValue(0, col, true);
            break;
        }
    }
    assert(!q.empty());
    while (!q.empty()) {
        Cell c = q.front();
        q.pop();
        assert(bm.getValue(c.m_row, c.m_col));
        for (int co = -1; co <= 1; co++) {
            for (int ro = -1; ro <= 1; ro++) {
                if (co != 0 && ro != 0)
                    continue;
                if (co == 0 && ro == 0)
                    continue;
                int col = c.m_col + co;
                int row = c.m_row + ro;
                if (row < 0 || row >= this->m_nrRows)
                    continue;
                if (col < 0 || col >= this->m_nrCols)
                    continue;
                if (!isBlackPoint(m_grid[row][col]) && !bm.getValue(row, col)) {
                    q.push(Cell(row, col));
                    bm.setValue(row, col, true);
                }
            }
        }
    }
    bool result = (bm.getNrTrueValues() == m_nrRows * m_nrCols - getNrBPs());
    return result;
}

bool RandomWriteState::hasIllegalWords() const {
    for (vector<WordSlot>::const_iterator it = m_slots.begin();
            it != m_slots.end(); it++) {
        if (!it->getPattern().isInstantiated())
            continue;
        if (it->getLength() < 3)
            continue;
        if (countMatchingWords(*it) == 0) {
            if (g_pm.getVerbosity() >= 3)
                cerr << "Illegal word in slot: " << it->getSimplePattern()
                << endl;
            return true;
        }
    }
    return false;
}

void RandomWriteState::writeHeatmap() const {
    ofstream ofile(g_pm.getHeatmapFileName().c_str(), ios::out);
    int heatmap[15][15];
    for (int col = 0; col < this->getNrCols(); col++)
        for (int row = 0; row < this->getNrRows(); row++)
            heatmap[col][row] = 0;
    for (auto slot : this->m_slots) {
        if (slot.getPattern().isInstantiated()) {
            if (this->wordIsThematic(slot))
                for (int idx = 0; idx < slot.getLength(); idx++) {
                    heatmap[slot.getCellRow(idx)][slot.getCellCol(idx)]++;
                }
        }
    }
    if (ofile) {
        for (int row = 0; row < this->getNrRows(); row++) {
            for (int col = 0; col < this->getNrCols(); col++) {
                if (isBlackPoint(m_grid[row][col]))
                    ofile << m_grid[row][col] << ' ';
                else if (m_grid[row][col] == BLANK)
                    ofile << "- ";
                else
                    ofile << heatmap[row][col] << ' ';
            }
            ofile << endl;
        }
        ofile.close();
    }
}

bool RandomWriteState::cannotReachThemPoints(int score) const {
    return false;
    bool result = (this->getMaxOptScore() < score);
    if (result && g_pm.getVerbosity() >= 3) {
        cerr << "Successor pruned as unable to exceed best current score:"
                << endl;
        cerr << *this;
        cerr << endl;
    }
    return result;
}

bool RandomWriteState::isLegal() const {
    bool result = true;
    string reason = "";
    if (adjacentBlackPoints()) {
        result = false;
        reason = "adjacent BPs";
    }
    if (hasIllegalWords()) {
        result = false;
        reason = "illegal word";
    }
    if (false && (!g_pm.getSymmetryInfo() && wordRepetition())) {
        reason = "word repetition";
        result = false;
    }
    if (exceedBlackPoints()) {
        result = false;
        reason = "exceed max BPs";
    }
    if (!this->isConnected()) {
        result = false;
        reason = "grid disconnected";
        //cerr << "Successor pruned away as illegal" << endl;
        //cerr << "Reason: " << reason << endl;
        //cerr << *this;
        //cerr << endl;
    }
    if (!result) {
        if (g_pm.getVerbosity() >= 3) {
            cerr << "Successor pruned away as illegal" << endl;
            cerr << "Reason: " << reason << endl;
            cerr << *this;
            cerr << endl;
        }
        return false;
    } else
        return true;
}

bool RandomWriteState::willExceedBPs() {
    int heurBPs = this->getHeurBPs();
    if (this->m_nrBlackPoints + heurBPs > MAX_BPS) {
        if (g_pm.getVerbosity() >= 3) {
            cerr << "Successor pruned with admissible estimates for future BPs:"
                    << endl;
            cerr << *this;
            cerr << endl << "Heur BPs: " << heurBPs << endl;
            cerr << endl;
        }
        return true;
    }
    return false;
}

int RandomWriteState::operator==(const RandomWriteState &rhs) const {
    if (this->getNrThematicPoints() != rhs.getNrThematicPoints())
        return 0;
    else
        return 1;
}

int RandomWriteState::operator<(const RandomWriteState &rhs) const {
    //cerr << "Comparing states with rule " << g_pm.getHeurOrder() << endl;
    if (g_pm.getHeurOrder() == 13) {
        double this_ratio = 1.0 * getF2();
        double other_ratio = 1.0 * rhs.getF2();
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 12) {
        double this_ratio = 1.0 * getF();
        double other_ratio = 1.0 * rhs.getF();
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 11) {
        double this_ratio = 1.0 * this->getPointsPerCell();
        double other_ratio = 1.0 * rhs.getPointsPerCell();
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 0) {
        double this_ratio = 1.0 * this->getNrFilledCells() / (1 + this->getNrBPs());
        double other_ratio = 1.0 * rhs.getNrFilledCells() / (1 + rhs.getNrBPs());
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 1) {
        double this_ratio = (1.0 * this->getMinSlotScore()) / (1 + this->getNrBPs());
        double other_ratio = (1.0 * rhs.getMinSlotScore() / (1 + rhs.getNrBPs()));
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 2) {
        double this_ratio = (1.0 * this->getNrThematicPoints());
        double other_ratio = (1.0 * rhs.getNrThematicPoints());
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 3) {
        double this_ratio = (1.0 * this->getNrThematicPoints() / (1 + this->getNrBPs()));
        double other_ratio = (1.0 * rhs.getNrThematicPoints() / (1 + rhs.getNrBPs()));
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 6) {
        double this_ratio = this->getNrThematicPoints();
        double other_ratio = rhs.getNrThematicPoints();
        if (g_pm.getVerbosity() >= 4) {
            cerr << "Comparing " << endl;
            cerr << *this << endl;
            cerr << rhs << endl;
            cerr << "Return " << ((this_ratio > other_ratio)) << endl;
        }
        if (this_ratio > other_ratio)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 7) {
        int diff = this->getNrThematicPoints() - rhs.getNrThematicPoints();
        if (true || this->getNonThemBudget() < 10) {
            if (diff < 0)
                return 0;
            if (diff > 0)
                return 1;
        }
        if (diff < 0)
            return 1;
        if (diff == 0 && this->m_heurScore < rhs.m_heurScore)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 5) {
        int diff = this->getNrThematicPoints() - rhs.getNrThematicPoints();
        if (diff < 0)
            return 1;
        if (diff == 0 && this->m_heurScore < rhs.m_heurScore)
            return 1;
        return 0;
    } else if (g_pm.getHeurOrder() == 4) {
        double this_ratio = (1.0 * this->getNrThematicPoints() * this->getMinSlotScore()) / (1 + this->getNrBPs());
        double other_ratio = (1.0 * rhs.getNrThematicPoints() * rhs.getMinSlotScore()) / (1 + rhs.getNrBPs());
        if (this_ratio < other_ratio)
            return 1;
        return 0;
    }
    assert(false);
    return 0;
    /*
    if (this->getMaxOptScore() < rhs.getMaxOptScore())
            return 1;
    if (this->getMaxOptScore() > rhs.getMaxOptScore())
            return 0;
    if (this->getNrThematicPoints() < rhs.getNrThematicPoints())
            return 1;
    if (this->getNrThematicPoints() > rhs.getNrThematicPoints())
            return 0;
     */
}
