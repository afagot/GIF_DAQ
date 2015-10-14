// **********************************************************************
// *   DAQ for the GIF
// *   Y.Benhammou, Alexis Fagot
// *   14/1/2015
// *********************************************************************

#include "../include/DataReader.h"
#include "../include/MsgSvc.h"
#include <limits>

using namespace std;

int main (int argc ,char *argv[])
{
    MSG_INFO("****************************************************\n");
    MSG_INFO("******    DAQ Program 16/09/15 Version 2.2    ******\n");
    MSG_INFO("****************************************************\n");

    DataReader *DR = new DataReader();

    /* Initialisation of the setup */

    MSG_INFO("[DAQ]: TDC INITIALISATION...");

    DR->Init("daqgifpp.ini");

    MSG_INFO("[DAQ]: INITIALISATION DONE");

    /* Run */

    //define a variable used to ask the user if he wants to start a new
    //run each time a run finishes. It is first initialised at "y" (yes)
    //taking in consideration that the user always wants to start the
    //first run.
    //Then it is changed to "default" and the user will be asked if he
    //wants to start a new run.
    //If the user wants to start a new run, we will leave him a bit of
    //time to update the run information into the configuration file
    string startrun = "y";

    while(startrun == "y"){
        startrun = "default";

        MSG_INFO("[DAQ]: RUN STARTED... \n");

        DR->Update();
        DR->Run();

        MSG_INFO("[DAQ]: RUN FINISHED\n");

        while(startrun != "y" && startrun != "n"){
            MSG_GREEN("[DAQ]: Would you like to start a new run? (y/n) ");
            cin >> startrun;
        }

        if(startrun == "y"){
            string emptystring;
            MSG_GREEN("[DAQ]: Update the configuration file then type OK and ENTER to continue...");
            cin >> emptystring;
        } else if (startrun == "n"){
            MSG_INFO("[DAQ]: GOODBYE!\n");
        }
    }
    return 0;
}
