// **********************************************************************
// *   DAQ for the GIF
// *   Y.Benhammou, Alexis Fagot
// *   14/1/2015
// *********************************************************************

#include "../include/DataReader.h"
#include "../include/MsgSvc.h"

using namespace std;

int main (int argc ,char *argv[])
{
    MSG_INFO("****************************************************\n");
    MSG_INFO("******    DAQ Program 29/07/15 Version 2.1    ******\n");
    MSG_INFO("****************************************************\n");

    DataReader *DR = new DataReader();

    DR->Init("daqgifpp.ini");

    /* START OF THE LOOP */
    MSG_INFO("[DAQ]: RUN STARTED... \n");

    DR->Run();

    MSG_INFO("[DAQ]: RUN FINISHED\n");
    return 0;
}
