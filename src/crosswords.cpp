#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>

/* for setpriority */
#include <sys/time.h>
#include <sys/resource.h>

#include "Experiment.h"

using namespace std;

int main23(int argc, char *argv[])
{
      std::cout << "\033[1;31mWombat version 2.0\033[0m\n";
      Experiment exp;

      if (argc > 1)
      {
            string arg = string(argv[1]);
            if (arg == "corner1")
                  exp.runDFSCorner(1);
            else if (arg == "corner2")
                  exp.runDFSCorner(2);
            else if (arg == "corner3")
                  exp.runDFSCorner(3);
            else if (arg == "corner4")
                  exp.runDFSCorner(4);
      }
      else
      {
            cerr << "Running DFS" << endl;
            exp.runDFS();
            //exp.evalPayloadPruning(options);
            //exp.runWithDetailedLog(options);
            //exp.evalDicSearch(options);
            //exp.evalDelayedStaticAnal(options);
            //exp.evalDeadlockDB(options);
            //exp.evalPruneWithBPs(options);
            //exp.stanardEval(options); // this runs the original code in the main function
      }
}
