/*
 *  untitled.cpp
 *  combus
 *
 *  Created by Adi Botea on 23/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "parammanager.h"
#include <assert.h>
#include <stdlib.h>

using namespace std;

ParamManager::ParamManager() :
m_solFileName("input-crosswords.pzl"), m_maxTime(MAX_CPU_TIME), m_maxNodes(MAX_DFS_NODES), m_maxSolutions(
MAX_DFS_SOLUTIONS), m_progagIters(PROPAG_ITERS), m_symmetricGrid(
SYMMETRIC_GRID), m_verbosity(VERBOSITY), m_maxBranchingFactor(
MAX_BRANCH_FACTOR), m_backjump(BACKJUMP), m_collectSlotStats(
COLLECT_SLOT_STATS), m_lookahead(LOOKAHEAD), m_dbSize(
CONFLICT_DB_SIZE), m_propagationOrdering(PROPAG_ORDER), m_solutionSampling(
SOL_SAMPL), m_dicPercentage(DIC_PERC), m_randomSeed(
RANDOM_SEED), m_nodeThresholdForRestart(THRESHOLD_RESTART), m_minThemPts(
MIN_THEM_POINTS), m_maxNonThemWords(MAX_NONTHEM_WORDS), m_pruneWithBPs(
PRUNE_WITH_BPS), m_delayedStaticAnal(DELAYED_STATIC_ANAL), m_payloadPruning(
PAYLOAD_PRUNING), m_heurOrder(HEUR_ORDER),
m_startCol(START_ROW_COL),
m_horizLen(HORIZ_VERT_LEN),
m_startRow(START_ROW_COL),
m_vertLen(HORIZ_VERT_LEN),
m_corner(CORNER),
m_slot_scoring(SLOT_SCORING),
m_maxNrBPMoves(MAX_NR_BP_MOVES),
m_ntw3(NTW3), m_ntw4(NTW4), m_ntw5(NTW5), m_ntw6(NTW6),
m_maxZeroCells(MAX_ZERO_CELLS),
m_tep(MAX_ZERO_CELLS),
m_search(SERIAL_DFS),
m_heurWeight(HEUR_WEIGHT),
m_expStyle(EXPANSION_STYLE) {
}

ParamManager::~ParamManager() {
}

void ParamManager::displayUsage() const {
    cerr << "Usage: ./wombat [-option value]+" << endl << "Options are:" << endl
            << "-i filename --> file with input data (mandatory)" << endl
            << "-o filename --> file where solutions are stored" << endl
            << "-sf filename --> file where the score is written" << endl
            << "-hf filename --> file where the score heatmap is written" << endl
            << "-t maxtime --> maximum search time in seconds (default infinity)"
            << endl
            << "-n maxnodes --> maximum number of expanded nodes (default infinity)"
            << endl
            << "-s maxsolutions --> maximum number of solutions (default infinity)"
            << endl << "-c nriter" << endl
            << "   --> nriter = 0 : no constraint propagation" << endl
            << "   --> nriter > 0: propagate constraints between cells and slots for nriter iterations"
            << endl
            << "  default: " << ParamManager::PROPAG_ITERS
            << "-g 1/0 --> whether across and down words are the same (default false)"
            << endl << "-v verbosity --> 0..3" << endl
            << "1 displays each 1000th visited node. 2 displays all visited nodes."
            << "  default: " << ParamManager::VERBOSITY
            << endl << "-b max branching factor (unbounded by default)" << endl
            << "-j 0/1 turn off/on deadlock driven backjumping"
            << endl << "  default: " << ParamManager::BACKJUMP << endl
            << "-ss 0/1 --> statistical computation of slot scores"
            << endl
            << "  default: " << ParamManager::COLLECT_SLOT_STATS << endl
            << "-l 0/1 --> successor ordering via lookahead"
            << endl << "  default: " << ParamManager::LOOKAHEAD
            << endl << "-d number --> maximal size of the deadlock database"
            << endl << "  default: " << ParamManager::CONFLICT_DB_SIZE << endl
            << "-po number --> ordering of variables in constraint propagation"
            << endl << "  --> 0 : partition vars into horizontal slots and vertical slots"
            << endl << "  --> 1 : use a stack" << endl
            << "  default: " << ParamManager::PROPAG_ORDER << endl
            << "-sampling 0/1 --> whether solution sampling is turned on."
            << endl << "  default: " << ParamManager::SOL_SAMPL
            << "-dp 0..100 --> percentage of the dictionary to be read in."
            << endl << "  default: " << ParamManager::DIC_PERC
            << "-dps int_value --> random seed to be used in dictionary filtering when dp != 100."
            << endl << "  default: " << ParamManager::RANDOM_SEED
            << endl
            << "-r int_value --> node threshold for restart"
            << endl << "  -1 means never restart"
            << endl
            << "  default: " << ParamManager::THRESHOLD_RESTART << endl
            << "-mts int_value --> min # of thematic points in a solution."
            << endl << "  default: " << ParamManager::MIN_THEM_POINTS << endl
            << "-mnw int_value --> max # of non-thematic words of length 5+ in a solution."
            << endl << "  default: " << ParamManager::MAX_NONTHEM_WORDS << endl
            << "-pbp 0/1 --> prune with admissible estimates of future BPs."
            << endl << "  default: " << ParamManager::PRUNE_WITH_BPS << endl
            << "-plp ratio --> prune with payload ratio."
            << endl << "  default: " << ParamManager::PAYLOAD_PRUNING << endl
            << "-tep int_value --> nr filled cells after which early pruning is activated."
            << endl << "  default: " << ParamManager::MAX_ZERO_CELLS << endl
            << "-dsa 0/1 --> delayed static analysis for generated states."
            << endl << "  default: " << ParamManager::DELAYED_STATIC_ANAL << endl
            << "-heur int_value --> heuristic to rank states." << endl
            << "  13 is for an admissible f value (described in AIIDE 22 paper)" << endl
            << "  12 is for (score - weight*sterile_load)" << endl
            << "  default: " << ParamManager::HEUR_ORDER << endl
            << "-corner int_value --> whether to build just one corner (and which one)."
            << endl << "  0 = no corner ; 1 = top left ; and so on clockwise"
            << endl << "  11 = instantiate only slots ending in *"
            << endl << "  default: " << ParamManager::CORNER << endl
            << "-slot_score int_value --> the style of slot scoring."
            << endl << "  default: " << ParamManager::SLOT_SCORING << endl
            << "-mz int_value --> max zero-score cells."
            << endl << "  default: " << ParamManager::MAX_ZERO_CELLS << endl
            << "-start_slot int_value --> id of the slot to start from."
            << endl << "  default: " << ParamManager::START_SLOT << endl
            << "-ntw3456 int_value int_value int_value int_value --> max nr non-them words of length 3, 4, 5 and 6."
            << endl << "  default: " << ParamManager::NTW3 << " " << ParamManager::NTW4
            << " " << ParamManager::NTW5 << " " << ParamManager::NTW6 << endl
            << "-search int_value --> search strategy."
            << endl << "  default: " << ParamManager::SERIAL_DFS << " for serial DFS" << endl
            << "  " << ParamManager::ALLSLOTS_MULTITHREAD << " independent DFS searches in multi-threading, each starting from a different slot" << endl
            << "  " << ParamManager::TWO_STEP_SEARCH << " for a two-stage approach (SoCS 21)" << endl
            << "  " << ParamManager::ITERATED_SEARCH << " for iterated searches with decreasing targets down from 215" << endl
            << "  " << ParamManager::SERIAL_BFS << " for Weighted A*" << endl
            << "  " << ParamManager::SERIAL_DFBNB << " for depth-first branch and bound" << endl
            << "-weight double_value --> weight of the heuristic for Weighted A* and other algorithms"
            << endl << "  default: " << ParamManager::HEUR_WEIGHT << endl
            << "-exp int_value --> state expansion: tiered (0) or standard (1)"
            << endl << "  default: " << ParamManager::EXPANSION_STYLE
            << endl;
}

bool ParamManager::readParams(int argc, char * const argv[]) {
    int it = 1;
    if (argc == 1) {
        this->displayUsage();
        return false;
    }
    while (it < argc) {
        string option = argv[it];
        if (option == "-i") {
            m_inputFileName = argv[it + 1];
            it += 2;
        } else if (option == "-sf") {
            m_scoreFileName = argv[it + 1];
            it += 2;
        } else if (option == "-hf") {
            m_heatmapFileName = argv[it + 1];
            it += 2;
        } else if (option == "-start_slot") {
            m_startSlot = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-o") {
            m_solFileName = argv[it + 1];
            it += 2;
        } else if (option == "-n") {
            m_maxNodes = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-t") {
            m_maxTime = atof(argv[it + 1]);
            it += 2;
        } else if (option == "-s") {
            m_maxSolutions = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-c") {
            m_progagIters = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-mz") {
            m_maxZeroCells = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-g") {
            m_symmetricGrid = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-v") {
            m_verbosity = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-b") {
            m_maxBranchingFactor = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-j") {
            m_backjump = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-ss") {
            m_collectSlotStats = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-po") {
            m_propagationOrdering = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-l") {
            m_lookahead = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-d") {
            m_dbSize = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-sampling") {
            m_solutionSampling = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-dp") {
            m_dicPercentage = atoi(argv[it + 1]);
            assert(0 < m_dicPercentage && m_dicPercentage <= 100);
            it += 2;
        } else if (option == "-dps") {
            m_randomSeed = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-r") {
            m_nodeThresholdForRestart = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-mnw") {
            this->m_maxNonThemWords = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-mts") {
            this->m_minThemPts = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-exp") {
            this->m_expStyle = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-ntw3456") {
            this->m_ntw3 = atoi(argv[it + 1]);
            this->m_ntw4 = atoi(argv[it + 2]);
            this->m_ntw5 = atoi(argv[it + 3]);
            this->m_ntw6 = atoi(argv[it + 4]);
            it += 5;
        } else if (option == "-pbp") {
            this->m_pruneWithBPs = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-plp") {
            this->m_payloadPruning = atof(argv[it + 1]);
            it += 2;
        } else if (option == "-weight") {
            this->m_heurWeight = atof(argv[it + 1]);
            it += 2;
        } else if (option == "-tep") {
            this->m_tep = atof(argv[it + 1]);
            it += 2;
        } else if (option == "-dsa") {
            this->m_delayedStaticAnal = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-heur") {
            this->m_heurOrder = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-slot_score") {
            this->m_slot_scoring = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-corner") {
            this->m_corner = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-search") {
            this->m_search = atoi(argv[it + 1]);
            it += 2;
        } else if (option == "-h") {
            this->displayUsage();
            return false;
        } else {
            cerr << "Unknown option or value: " << argv[it] << endl;
            return false;
        }
    }
    return true;
}
