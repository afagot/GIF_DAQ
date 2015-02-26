// **********************************************************************
// *   DAQ for the GIF 
// *   Y.Benhammou, Alexis Fagot
// *   14/1/2015
// *********************************************************************

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include <map>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/io.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"
#include "../include/CAENVMEtypes.h"

#include "../include/v1718.h"
#include "../include/v1190a.h"
#include "../include/DataReader.h"
#include "../include/IniFile.h"
#include "../include/MsgSvc.h"

using namespace std;

int main (int argc ,char *argv[])
{
    MSG_INFO("****************************************************\n");
    MSG_INFO("******    DAQ Program 14/1/15 Version 1.00    ******\n");
    MSG_INFO("****************************************************\n");

    DataReader *DR = new DataReader();

    DR->Init("daqgifpp.ini");

    /* START OF THE LOOP */
    MSG_INFO("RUN STARTED... \n");

    DR->Run();

    MSG_INFO("RUN FINISHED\n");
    return 0;
}
