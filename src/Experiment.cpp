/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Experiment.cpp
 * Author: adi
 * 
 * Created on 23 December 2017, 18:42
 */

//#include <c++/4.8.2/functional> // does not compile for me

#include "Experiment.h"
#include "parammanager.h"

#include "randomwritestate.h"
#include "deadlockrecord.h"
#include "util.h"
#include "depthfirstsearch.h"
#include "bestfirstsearch.h"
#include "dfbnb.h"
#include "globals.h"
#include "limiteddiscrsearch.h"
#include <pthread.h>
#include <exception>

Experiment::Experiment()
{
}

Experiment::Experiment(const Experiment &orig)
{
}

Experiment::~Experiment()
{}

void *worker_thread(void *arg)
{
    //cerr << "This is worker_thread #" << (long)arg << endl;
    Experiment e;
    long result = e.runDFS((long)arg);
    pthread_exit((void*)result);
}

int Experiment::runDFSMultithreads() {
    int N = 100;
    pthread_t my_thread[100];
    long id;
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    for(id = 0; id < N; id++) {
        int ret =  pthread_create(&my_thread[id], NULL, worker_thread, (void*)id);
        if(ret != 0) {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
        }
    }
    int retval = TIMEOUT;
    for(id = 0; id < N; id++) {
        void *returnvalue;
        pthread_join(my_thread[id], &returnvalue);
        if (returnvalue != TIMEOUT)
            retval = (long int) returnvalue;
        //cerr << "Thread " << id << " returned with value " << (long) returnvalue << endl;
    }
    //cerr << "Completed wombat run in multithreading mode" << endl;
    pthread_exit(NULL);
    return retval;
}

int Experiment::incrementScore(int v) {
    while (true) {
        //int result = runDFSMultithreads();
        int result = runDFS();
        if (result == PROVED_NO_SOL)
            break;
        g_pm.incrMinThemPts(v);
    }
    return 0;
}

int Experiment::twoPhaseSearch() {
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    vector<Move> moves;
    int target = 280;
    while (target >= 200) {
        cerr << "Target = " << target << endl;
        RandomWriteState state(1, g_pm.getStartSlot());
        Node root(state, 0, 0);
        g_pm.setMinNrThemPoints(target);
        DepthFirstSearch search(root, "summary.txt");
        search.runSearch();
        if (search.getNrSolsFound() == -1) {
            moves = search.getBestPartSol().getData().getMoves();
            if (moves.size() >= 12)
                break;
        }
        target -= 5;
    }
    if (moves.size() < 12)
        return -1;
    int maxnrpoints = 220, minnrpoints = 140;
    while (maxnrpoints - minnrpoints > 1) {
        int nrpoints = (maxnrpoints + minnrpoints)/2;
        g_pm.setMinNrThemPoints(nrpoints);
        cerr << "Target score " << nrpoints << endl;
        RandomWriteState state2(1, g_pm.getStartSlot());
        for (unsigned int i = 0; i < moves.size() - 6; i++) {
            Move move = moves[i];
            state2.makeMove(move.row, move.col, move.direction, move.word);
        }
        Node root2(state2, 0, 0);
        DepthFirstSearch search2(root2, "summary.txt");
        int result = search2.runSearch();
        if (search2.getNrSolsFound() > 0)
            minnrpoints = nrpoints;
        if (search2.getNrSolsFound() <= 0)
            maxnrpoints = nrpoints;
    }
    return 0;
}

int Experiment::twoPhaseSearch2() {
    time_t start;
    time(&start);

    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    vector<Move> moves;
    RandomWriteState bestPartSol = RandomWriteState();
    int target_score = 240;
    int trim_size = 3;
    while (target_score >= 180) {
        cerr << "===========================================" << endl;
        cerr << "Target score in overestimation search = " << target_score << endl;
        RandomWriteState state(1, g_pm.getStartSlot());
        Node root(state, 0, 0);
        g_pm.setMinNrThemPoints(target_score);
        DepthFirstSearch search(root, "summary.txt");
        search.runSearch();
        bestPartSol = search.getBestPartSol().getData();
        if (search.getNrSolsFound() == -1) {
            moves = search.getBestPartSol().getData().getMoves();
            if (moves.size() >= 15) {
                break;
            }
        }
        if (search.getNrSolsFound() == 0)
            break;
        target_score -= 5;
    }    
    //bestPartSol.writeToFile("bps-" + g_pm.getInputFileName());
    if (moves.size() < 10) {
        cerr << "Unable to extract initial moves. Aborting search." << endl;
        return -1;
    } else {
        cerr << "Moves extracted from over-estimation search: ";
        for (unsigned int i = 0; i < moves.size()*.6; i++)
            cerr << moves[i] << " ";
        cerr << endl;
    }
    target_score = 215;
    string summary;
    while (target_score >= 160) {
        g_pm.setMinNrThemPoints(target_score);
        cerr << "========================================" << endl;
        cerr << "Target score in exact score search = " << target_score << endl;
        RandomWriteState state2(1, g_pm.getStartSlot());
        for (unsigned int i = 0; i < moves.size()*.6; i++) {
            Move move = moves[i];
            state2.makeMove(move.row, move.col, move.direction, move.word);
        }
        state2.writeToFile("ips-" + g_pm.getInputFileName());
        Node root2(state2, 0, 0);
        DepthFirstSearch search2(root2, "summary.txt");
        int result = search2.runSearch();
        summary = search2.getSummaryInfo();
        if (search2.getNrSolsFound() > 0) {
            bestPartSol = search2.getBestPartSol().getData();
            bestPartSol.writeToFile("sol-" + g_pm.getInputFileName());
            break;
        }
        target_score--;
    }
    time_t current;
    time(&current);
    double result = difftime(current, start);
    assert (result >= 0.0);
    cerr <<  summary << " Total time in seconds: " << result << endl;
    return 0;
}

int Experiment::twoPhaseSearch4() {
    time_t start;
    time(&start);

    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    vector<Move> moves;
    RandomWriteState bestPartSol = RandomWriteState();
    int target_score = 200;
    g_pm.setMinNrThemPoints(target_score);
    for (int mz = 2; mz <= 4; mz++) {
        g_pm.setMaxZeroCells(mz);
        cerr << "===========================================" << endl;
        cerr << "Max zero cells in overestimation search = " << mz << endl;
        RandomWriteState state(1, g_pm.getStartSlot());
        Node root(state, 0, 0);
        DepthFirstSearch search(root, "summary.txt");
        search.runSearch();
        bestPartSol = search.getBestPartSol().getData();
        if (search.getNrSolsFound() == -1) {
            moves = search.getBestPartSol().getData().getMoves();
            if (moves.size() >= 15) {
                break;
            }
        }
        if (search.getNrSolsFound() == 0)
            break;
    }    
    //bestPartSol.writeToFile("bps-" + g_pm.getInputFileName());
    if (moves.size() < 10) {
        cerr << "Unable to extract initial moves. Starting systematic searches from scratch." << endl;
        moves.clear();
    } else {
        cerr << "Moves extracted from over-estimation search: ";
        for (unsigned int i = 0; i < moves.size()*.4; i++)
            cerr << moves[i] << " ";
        cerr << endl;
    }
    target_score = 180;
    string summary;
    g_pm.setMaxZeroCells(169);
    while (target_score >= 160) {
        g_pm.setMinNrThemPoints(target_score);
        cerr << "========================================" << endl;
        cerr << "Target score in exact score search = " << target_score << endl;
        RandomWriteState state2(1, g_pm.getStartSlot());
        for (unsigned int i = 0; i < moves.size()*.4; i++) {
            Move move = moves[i];
            state2.makeMove(move.row, move.col, move.direction, move.word);
        }
        state2.writeToFile("ips-" + g_pm.getInputFileName());
        Node root2(state2, 0, 0);
        DepthFirstSearch search2(root2, "summary.txt");
        int result = search2.runSearch();
        summary = search2.getSummaryInfo();
        if (search2.getNrSolsFound() > 0) {
            bestPartSol = search2.getBestPartSol().getData();
            bestPartSol.writeToFile("sol-" + g_pm.getInputFileName());
            break;
        }
        target_score--;
    }
    time_t current;
    time(&current);
    double result = difftime(current, start);
    assert (result >= 0.0);
    cerr <<  summary << " Total time in seconds: " << result << endl;
    return 0;
}

int Experiment::searchWithTargetIterations() {
    time_t start;
    time(&start);

    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    vector<Move> moves;
    RandomWriteState bestPartSol = RandomWriteState();
    int target_score = 215;
    string summary;
    while (target_score >= 160) {
        g_pm.setMinNrThemPoints(target_score);
        cerr << "========================================" << endl;
        cerr << "Target score in exact score search = " << target_score << endl;
        RandomWriteState state2(1, g_pm.getStartSlot());
        Node root2(state2, 0, 0);
        DepthFirstSearch search2(root2, "summary.txt");
        int result = search2.runSearch();
        summary = search2.getSummaryInfo();
        if (search2.getNrSolsFound() > 0) {
            bestPartSol = search2.getBestPartSol().getData();
            bestPartSol.writeToFile("sol-" + g_pm.getInputFileName());
            break;
        }
        target_score--;
    }
    time_t current;
    time(&current);
    double result = difftime(current, start);
    assert (result >= 0.0);
    cerr <<  summary << " Total time in seconds: " << result << endl;
    return 0;
}

int Experiment::runDFS(int start_slot)
{
    //cerr << "Start slot = " << start_slot << endl;
    RandomWriteState state(1, start_slot);
    if (state.shortStartSlot())
        return SHORT_START_SLOT;
    Node root(state, 0, 0);
    DepthFirstSearch search(root, "summary.txt");
    search.runSearch();
    if (search.getNrSolsFound() == -1)
        return PROVED_NO_SOL;
    else if (search.getNrSolsFound() > 0)
        return SOL_FOUND;
    else {
        assert (search.getNrSolsFound() == 0);
        return TIMEOUT;
    }
}

int Experiment::runBFS()
{
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    RandomWriteState state(1, g_pm.getStartSlot());

    Node root(state, 0, 0);
    BestFirstSearch search(root, "summary.txt");
    search.runSearch();
    if (search.getNrSolsFound() == -1)
        return PROVED_NO_SOL;
    else if (search.getNrSolsFound() > 0)
        return SOL_FOUND;
    else {
        assert (search.getNrSolsFound() == 0);
        return TIMEOUT;
    }
}

int Experiment::runDFBnB()
{
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    RandomWriteState state(1, g_pm.getStartSlot());
    Node root(state, 0, 0);
    DFBnB search(root, "summary.txt");
    search.runSearch();
    if (search.getNrSolsFound() == -1)
        return PROVED_NO_SOL;
    else if (search.getNrSolsFound() > 0)
        return SOL_FOUND;
    else {
        assert (search.getNrSolsFound() == 0);
        return TIMEOUT;
    }
}

int Experiment::runDFS()
{
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    g_pm.setSlotScoring(5);
    g_pm.setPruneWithBPs(false);
    RandomWriteState state(1, g_pm.getStartSlot());
    Node root(state, 0, 0);
    DepthFirstSearch search(root, "summary.txt");
    search.runSearch();
    if (search.getNrSolsFound() == -1)
        return PROVED_NO_SOL;
    else if (search.getNrSolsFound() > 0)
        return SOL_FOUND;
    else {
        assert (search.getNrSolsFound() == 0);
        return TIMEOUT;
    }
}

void Experiment::runDFSCorner(int corner)
{
    g_pm.setCorner(corner);
    g_pm.setHeurOrder(3);
    g_pm.setVerbosity(1);
    g_pm.setDBSize(0);
    g_pm.setMaxNrBPMoves(0);
    g_pm.setPayloadPruning(false);
    g_pm.setSubarea(0, 13, 0, 13);
    g_pm.setMaxNonThemWords(2);
    g_pm.setPruneWithBPs(false);
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1, g_pm.getStartSlot());
    //state.addCharacter(12, 0, 'a');
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               -1, //g_pm.getMaxNodes(),
               -1,  //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}

void Experiment::evalPayloadPruning()
{
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1, g_pm.getStartSlot());
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    g_pm.setVerbosity(0);
    g_pm.setDBSize(1400);
    g_pm.setPayloadPruning(true);
    g_pm.setMaxNonThemWords(4);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               g_pm.getMaxNodes(),
               1, //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               true); //g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}

void Experiment::runWithDetailedLog()
{
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1, g_pm.getStartSlot());
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    g_pm.setVerbosity(3);
    g_pm.setDBSize(1400);
    g_pm.setPayloadPruning(true);
    //g_pm.setMaxNonThemWords(4);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               g_pm.getMaxNodes(),
               1, //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               true); //g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}

void Experiment::addLeftTopBlackpoints() const {
    cerr << "Generating initial grids by adding blackpoints to the top-left of the original wall" << endl;
    State::readStaticInfo(g_pm.getInputFileName());
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1, g_pm.getStartSlot());
    state.collectSlots();
    state.addLeftTopBlackpoints(0);
}

