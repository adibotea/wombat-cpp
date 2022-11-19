#include <iostream>
#include "util.h"
#include "parammanager.h"
#include <stdlib.h>
#include "globals.h"
#include "Experiment.h"
#include <assert.h>

using namespace std;

int main(int argc, char * const argv[]) {
    std::cout << "\033[1;31mWombat version 3.0\033[0m\n";
    if (!g_pm.readParams(argc, argv)) {
        g_pm.displayUsage();
        exit(0);
    }
    Experiment exp;
    if (g_pm.getSearch() == ParamManager::SERIAL_BFS) {
        exp.runBFS();
    } else if (g_pm.getSearch() == ParamManager::SERIAL_DFS) {
        exp.runDFS();
    } else if (g_pm.getSearch() == ParamManager::SERIAL_DFBNB) {
        exp.runDFBnB();
    } else if (g_pm.getSearch() == ParamManager::ALLSLOTS_MULTITHREAD) {
        exp.runDFSMultithreads();
    } else if (g_pm.getSearch() == ParamManager::TWO_STEP_SEARCH) {
        exp.twoPhaseSearch2();
    } else if (g_pm.getSearch() == ParamManager::TWO_STEP_SEARCH4) {
        exp.twoPhaseSearch4();
    } else if (g_pm.getSearch() == ParamManager::ITERATED_SEARCH) {
        exp.searchWithTargetIterations();
    } else if (g_pm.getSearch() == ParamManager::TOP_LEFT_BPS) {
        exp.addLeftTopBlackpoints();
    }
    cout.flush();
    return 0;
}
