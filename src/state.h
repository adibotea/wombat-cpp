/*
 *  state.h
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STATE_H_
#define _STATE_H_

#include "dictionary.h"
#include "wordslot.h"
#include "multicharmap.h"
#include <iostream>
#include <map>
#include <math.h>
#include <assert.h>
#include "deadlockrecord.h"
#include "staticslotstats.h"
#include "booleanmap.h"
#include "constants.h"
#include <sstream>
#include "move.h"
#include <algorithm>

using namespace std;

/*
 * TODO: Looks like this class has too much static info.
 * Should see if this can be implemented somewhere else.
 */
class State {
public:
    State();
    State(int iters, int start_slot);
    virtual ~State() {
    };
    int getNTW(int idx) const {
    	assert (0 <= idx && idx < MAX_WORD_LENGTH);
    	return this->m_nrNTW[idx];
    }
    void makeMove(int row, int col, bool direction, string word);
    void addBlackPoint(int row, int col);
    void addBlackPoint2(int row, int col);
    bool isGoal() const {
        return this->m_goal;
    }
    int getF() const {
        return m_f;
    }
    int getF2() const {
        return m_f2;
    }

    int countSterileCommittments() const;
    int countBlanksAdjToLetters() const;
    int countBlanksAdjToSlot(int slot_idx) const;
    double weightedSumLetters(int slot_idx) const;
    
    void addCharacter(int row, int col, char character) {
        assert(0 <= row && row < m_nrRows &&
                0 <= col && col < m_nrCols);
        assert (m_grid[row][col] == BLANK);
        m_grid[row][col] = character;        
        m_nrFilledCells++;
    }
    
    double getPointsPerCell() const {
        double filledCells = getNrFilledCells() - getNrBPs() + countSterileCommittments();
        return 1.0*getNrThematicPoints()/(filledCells + 1.0);
    }

    bool backjumpHere(const State & deadlockState) const;

    void addLeftTopBlackpoints(int slot_idx) const;

    /*
     * Preserve the existing slots, except for those
     * impacted by the most recently added black point.
     * Split the impacted ones into sub-slots.
     * Side effect: the id of the most recently instantiated slot
     * may become corrupted. Not sure if it's used in the DBP program anyways.
     */
    void updateSlots();
    void collectSlots();
    void computeDeadlockInfo(BooleanMap & boolMap);

    bool isSterileOk(const WordSlot & slot) const;

    int getEstimBPs(const WordSlot & slot);

    int getNrThematicPoints() const {
        return m_nrThematicPoints;
    }
    
    int getNonThemBudget() const {
        return this->m_sterileBudget;
    }

    int getCellBudget() const {
        return this->m_cellBudget;
    }

    int getMaxFuturePoints() const {
        return this->m_maxFuturePoints;
    }

    int getMaxOptScore() const {
        return this->m_maxFuturePoints + this->m_nrThematicPoints;
    }

    int getNrFilledCells() const {
        return this->m_nrFilledCells;
    }
    string selPatAsString() const;

    static int getNrRows() {
        return m_nrRows;
    }

    static int getNrCols() {
        return m_nrCols;
    }

    static Statistics getPropIterationStats() {
        return m_propIterationStats;
    }
    
    const vector<WordSlot> & getDeadlockSlots() const {
        return m_deadlockSlots;
    };

    const DeadlockRecord & getDeadlockCells() const {
        return m_deadlock;
    };

    DeadlockRecord getPartialDeadlock() const {
        return m_deadlock.difference(m_recentCellInstantiations);
    }

    bool getSkipDeadlockExtraction() const {
        return m_skipDeadlockExtraction;
    }
    string getRow(int i) const;
    string getCol(int j) const;

    const vector<Cell> & getSelectedSlotCells() const {
        return m_slots[m_idxSelSlotGen].getCells();
    }

    const vector<Cell> & getRecentCellInstantiations() const {
        return m_recentCellInstantiations;
    }

    bool isDeadlock() const;
    int getNrBPBlocks() const {
        return this->m_nrBPBlocks;
    }
    int getNrBPs() const {
        return this->m_nrBlackPoints;
    }
    bool isGridComplete() const;
    void makeMoveSwapFlag(const WordSlot &slot);
    void makeMove(const WordSlot & slot, string word);
    void makeHorizMove(int row, int col, string word);
    void makeVertMove(int row, int col, string word);
    bool matchesCellPattern(const vector<CellRecord> & deadlock) const;
    /* print a summary with # of variables of each length and # of constraints. */
    void printVariableInfo(ostream & os) const;
    static void readDictionaries(ifstream & file);
    static void readHeader(ifstream & file);
    static void readInitGrid(ifstream & file);
    static void readStaticInfo(const string & filename);

    void setDeadlockCells(const DeadlockRecord & record) {
        m_deadlock = record;
    };
    
    void propagateConstraints(int nrIterations);

    void setPropagFromScratch(bool value) {
        m_procFromScratch = value;
    };

    void setPropagationIterations(unsigned int value) {
        m_propagationIterations = value;
    }

    void setSkipDeadlockExtraction(bool value) {
        m_skipDeadlockExtraction = value;
    }
    void updateSlotsFromGrid();
    void updateSlotFromGrid(WordSlot & slot);

    /*
     * For the slot given as an argument, return a vector of positions
     * where placing a BP would be legal. A "position" (i.e., a value 
     * in the result vector) is the offset in the slot.
     * E.g., a value of 3 is location 3 from the beginning of the slot
     * (i.e., the fourth location in the slot).
     */
    vector<int> getLegalBPPos(const WordSlot & slot) const;

    /*
     * Return true if adding a black point at that location is legal,
     * and false otherwise. 
     */
    bool isBPLegal(int row, int col) const;

    /*
     * Return the last word added to this state
     */
    string getLastWord() const {
        return this->m_lastWord;
    }

    void setLastWord(string s) {
        this->m_lastWord = s;
    }

    /*
     * Add all forced that can be BPs recognized.
     * Return true iff at least one BP is added.
     */
    bool addForcedBPs();
    bool addAtMostOneForcedBP();
    void performStaticAnalysis();
    double getMinSlotScore() const {
        return this->m_minSlotScore;
    }
    int computeSterileLoad();
    std::vector<int> markCommittedCells() const;
    int getNrZeroCells() const {
        return this->m_nrZeroCells;
    }
    int getNrSterileSlots() const {
        return this->m_nrSterileSlots;
    }
    int getNrOpenSlots() const {
        return this->m_nrOpenSlots;
    }
    int getNrThemSlots() const {
        return this->m_nrThemSlots;
    }

    bool shortStartSlot() const;

    std::vector<Move> getMoves() const {
        return this->m_moves;
    }

    void writeToFile(std::string filename) {
        ofstream myfile;
        myfile.open (filename);
        for (int row = 0; row < this->getNrRows(); row++) {
            string s = this->getRow(row);
            std::replace(s.begin(), s.end(), '@', '#');
            std::replace(s.begin(), s.end(), ' ', '.');
            myfile << s << endl;
        }
        myfile.close();
    }

    void writeToFile2(ofstream & ofile) const {
        for (int row = 0; row < this->getNrRows(); row++) {
            string s = this->getRow(row);
            ofile << s << endl;
        }
        ofile << endl;
    }

    void writeToFilePzl(std::string filename) const {
        ofstream myfile;
        myfile.open (filename);
        myfile << 13 << endl << 13 << endl << 1 << endl << 0 << endl << 0 << endl;
        for (int row = 0; row < this->getNrRows(); row++) {
            string s = this->getRow(row);
            for (auto c : s) {
                myfile << c << ' ';
            }
            myfile << endl;
        }
        myfile << 2 << endl << 1 << endl << "them-dic-21.txt" << endl;
        myfile << 1 << endl << 1 << endl << 0 << endl;
        myfile << "dictionary.txt" << endl << 1 << endl << 1 << endl;
    }

    bool slotEndsWithOrigBP(const WordSlot & slot) const;

protected:

    typedef enum {
        INSTANTIATION, CONSTRAINT
    } cellUpdateType;
    static unsigned int m_nrRows;
    static unsigned int m_nrCols;
    //unsigned int m_maxBlackPoints;
    static vector<Dictionary> m_horizDics;
    static vector<Dictionary> m_vertDics;
    static bool m_allowNewBlackPoints;
    static bool m_allowTwoChars[2];
    static char m_initGrid[MAX_NR_ROWS][MAX_NR_COLS];
    static unsigned int m_conflictDetection;
    //static vector<StaticSlotStats> m_slotStats;
    static Statistics m_propIterationStats;

    string m_lastWord; // last word added to this state
    // map that tells what characters are still allowed for each cell;
    // this is the low-level problem encoding
    MultiCharMap m_multiCharMap;
    // vector of cells affected by the last move
    // (i.e., either a letter or a black cell has been added)
    vector<Cell> m_recentCellInstantiations;
    vector<Cell> m_recentCellConstraints;
    unsigned int m_propagationIterations;
    static int m_propagationOrdering;
    unsigned int m_nrBlackPoints;
    int m_nrThematicPoints;
    unsigned int m_nrFilledCells;
    double m_minSlotScore;
    int m_oldSlotIdx; 
    int m_idxSelSlotGen;
    // position around which to build subslots in the selected slot
    int m_posInSelSlot;
    vector<int> m_themSlotOrder;
    bool m_wordRepetition;
    bool m_complete;
    unsigned int m_recentBlackPointRow;
    unsigned int m_recentBlackPointCol;
    char m_grid[MAX_NR_ROWS][MAX_NR_COLS];
    map<string, string> m_wordsInGrid;
    DeadlockRecord m_deadlock;
    vector<WordSlot> m_deadlockSlots;
    int m_previousInstantiatedSlot;
    vector<WordSlot> m_slots;
    bool m_procFromScratch;
    bool m_skipDeadlockExtraction;
    /* True iff a slot with zero hits has been detected. */
    bool m_0HitSlot;
    bool m_adjBPs; /* true if there are adjacent BPs */
    /*
     * Number of pairs (cell, direction) where thematic points cannot 
     * possibly occur.
     * 
     */
    int m_maxFuturePoints;
    /*
     * Number of non-thematic words of length at least MIN_LENGTH_NTW
     */
    int m_nrSterileWords;
    /* 
     * Number of non-thematic words of a given length 
     */
    int m_nrNTW[MAX_WORD_LENGTH];
    /*
     * A BP block is a connected set of BPs, where two BPs are connected
     * if they share a common corner.
     */
    int m_nrBPBlocks;
    /*
     * The number of pairs (cell,direction) with no points.
     * Black points not included in the counting.
     */
    //int m_sterileLoad;
    /*
     * Number of slots of a given length
     */
    vector<int> m_nrSlots;

    int m_heurBPs; // estimate # of BPs needed in the future

    bool m_goal;
    
    bool m_initGridFlag;
    
    int m_sterileBudget;
    int m_cellBudget;
    int m_sterileLoad;
    int m_f;
    int m_f2;
    
    int m_nrThemSlots;
    int m_nrSterileSlots;
    int m_nrOpenSlots;

    int m_heurScore;

    // Cells with 0 points
    int m_nrZeroCells;
    // Cells with 2 points
    int m_nr2PointCells;

    // history of moves
    std::vector<Move> m_moves;

    /*
     * Return a heuristic admissible estimation of
     * the number of BPs needed to complete the grid
     */
    int getHeurBPs() const {
    	return this->m_heurBPs;
    }
    void markBPFreeSlots();
    void computeDeadlockInfo(int seedSlotIdx, BooleanMap & boolMap);
    void computeDeadlockCells(const vector<int> & slots);

    bool exceedBlackPoints() const {
        return (m_nrBlackPoints > MAX_BPS);
    };
    bool adjacentBlackPoints() const {
        if (m_nrBlackPoints <= 1)
            return false;
        return this->m_adjBPs;
    };
    bool adjacentBlackPoints(int row, int col) const;
    void collectSlotStatistics();
    void collectSlotsHoriz(int row);
    void collectSlotsVert(int col);
    int computeMaxPatternLength() const;
    double getSlotThemScore(int it) const {
        return m_slots[it].getNrThemWords();
    }
    double computeSlotScore(int it) const;
    /*
     * Counts only the words of the same length as the argument.
     */
    int countMatchingWords(const WordSlot & pattern) const;
    bool hasMatchingWord(const WordSlot & slot) const;
    int getFirstHorizBlank(int row) const;
    int getFirstVertBlank(int col) const;
    vector<int> getIntersectingSlots(int slotIdx) const;
    void getPatternCharacterMap(WordPattern & pattern) const;
    bool wordRepetition() const {
        return m_wordRepetition;
    };
    bool semiClosure() const;
    void updateMultiCharMap(const WordSlot & slot);
    void updateMultiCharMap();
    /**
     * A propagation technique tailored for grids with dynamic black points.
     * It works as follows. 
     * Given a slot, let the "first black point" be the first position
     * in the slot so that a valid word can be placed in the previous cells
     * (i.e., the cells before the first black point position).
     * We call these cells the constrained prefix. 
     * We perform the propagation only through the cells that belong to 
     * constrained prefixes.
     **/
    void propConstDBP(int nrIterations);
    bool slotContainsRecentlyUpdatedCell(const WordSlot & slot,
            cellUpdateType updateType) const;
    void computeRecentlyConstrainedCells(
            const int m1[MAX_NR_ROWS][MAX_NR_COLS],
            const int m2[MAX_NR_ROWS][MAX_NR_COLS]);
    bool wordIsThematic(const WordSlot & slot) const;
    /*
     * Iterate through the slots that are fully instantiated.
     * Collect statistics such as the number of thematic letters,
     * and the number of non-thematic words of length greater or equal to
     * MIN_LENGTH_NTW;
     * Also check word repetition.
     * Also set the number of matching words to 1 for each instantiated slot.
     */
    void analyseInstantiatedSlots();
    /*
     * Iterate through the slots that are not fully instantiated,
     * and have a length of at least 3.
     * Set the number of matching words for each slot.
     */
    void analyseUninstantiatedSlots();
    /*
     * When new black points are allowed, perform the so-called 0-hit test.
     * In such a test, we check whether there exists a slot with a
     * sequence of three or more positions that cannot possibly belong to any word
     * from the dictionary.
     */
    void perform0HitTest(WordSlot & slot, vector<Dictionary> & dics,
            bool non_them_ok, vector<int> & allowed_bps);
};

#endif
