/*
 *  randomwritestate.h
 *  combus
 *
 *  Created by Adi Botea on 7/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _RANDOM_WRITE_STATE_H_
#define _RANDOM_WRITE_STATE_H_

#include "state.h"
#include "wordslot.h"
#include "parammanager.h"
#include <iostream>

class RandomWriteState : public State {
public:
    RandomWriteState();
    RandomWriteState(int iters, int start_slot);
    ~RandomWriteState();
    vector<RandomWriteState> generateSuccessors(int bestSolScore,
            bool only_them_words, bool prune_with_bps);
    bool isLegal() const;
    bool willExceedBPs();
    bool staticAnalAndTest(double bestSolScore, bool prune_with_bps);
    /*
     * Return true if it's guaranteed that this state cannot reach the 
     * score given as an argument
     */
    bool cannotReachThemPoints(int score) const;
    int computeHeurBPsSlow() const;
    /*
     * Return true if the current thematic score is higher than
     * a given fraction of the total number of non-blank cells
     */
    bool isPromising() const;
    int operator==(const RandomWriteState &rhs) const;
    int operator<(const RandomWriteState &rhs) const;
    int getNrAllSuccessors() const {
        return m_nrAllSuccessors;
    }
    /*
     * Return true iff the BPs do not disconnect the grid
     */
    bool isConnected() const;

    void writeHeatmap() const;

private:
    /*
     * Generate successors.
     * Consider not only words of the length of the slot,
     * but also shorter words.
     * AL stands for arbitrary length
     */
    vector<RandomWriteState> genSuccsAL(const WordSlot & slot, int bestSolScore,
            bool only_them_words, bool prune_with_bps);
    vector<RandomWriteState> genSuccsPartDelay(const WordSlot & sel_slot,
    		int bestSolScore,
            bool only_them_words, bool prune_with_bps);
    vector<RandomWriteState> genSuccsStd(const WordSlot & sel_slot,
    		int bestSolScore,
            bool only_them_words, bool prune_with_bps);

    bool hasIllegalWords() const;
    /*
     * Try to create a successor state where just a BP is added.
     * Return false when the BP cannot be added, or adding the BP leads to
     * a recognized deadend.
     * The successor state is available in the last argument.
     */
    bool makeBPOnlySucc(const WordSlot & slot, int bestSolScore,
    		bool prune_with_bps,
    		RandomWriteState & result);

    int m_nrAllSuccessors;
    int m_nrBPMoves;
};

#endif
