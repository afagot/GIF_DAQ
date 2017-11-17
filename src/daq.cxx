// **********************************************************************
// *   DAQ for the GIF
// *   Y.Benhammou, Alexis Fagot
// *   14/1/2015
// *********************************************************************

#include "../include/DataReader.h"
#include "../include/utils.h"
#include "../include/MsgSvc.h"
#include <limits>
#include <fstream>

using namespace std;

int main (int argc ,char *argv[])
{
    //The program needs 2 arguments to run : its own name and the path to the
    //log file that is written inside the log file in the RUN directory. The
    //will then be used by the MSG functions to know where to write the logs.
    if(argc == 2){
        ofstream logpathfile(__logpath.c_str(), ios::out);
        logpathfile << argv[1];
        logpathfile.close();
    } else {
        SendDAQError();
        exit(EXIT_FAILURE);
    }

    MSG_INFO("                                                    ");
    MSG_INFO("                                                    ");
    MSG_INFO("                                                    ");
    MSG_INFO("           *****       *****   *****                ");
    MSG_INFO("        ***********    *****   *****                ");
    MSG_INFO("      **************   *****   *****                ");
    MSG_INFO("     ******     *****  *****   *******              ");
    MSG_INFO("    ******    ******   *****   *******              ");
    MSG_INFO("    ******   ******    *****   *****                ");
    MSG_INFO("     ******            *****   *****   *            ");
    MSG_INFO("      ******  *****    *****    ***** ***           ");
    MSG_INFO("        **********     *****     *********          ");
    MSG_INFO("            ***                    ****             ");
    MSG_INFO("                                         **     **  ");
    MSG_INFO("                                       ****** ******");
    MSG_INFO("                                         **     **  ");
    MSG_INFO("                                                    ");
    MSG_INFO("                                                    ");
    MSG_INFO("****************************************************");
    MSG_INFO("******    DAQ Program 16/11/17 Version 4.0    ******");
    MSG_INFO("****************************************************");

    DataReader *DR = new DataReader();

    //The DAQ is communicating with the WEB DCS. It reads a file containing
    //run commands from the DCS (RAMP, WAIT, START and STOP) and gives feedback
    //on the run status (DAQ_RDY, RUNNING, NEXT).
    //When the DAQ gets START, the run starts and a busy is sent via RUNNING.
    //When the run ends, the DAQ sends NEXT and waits for the next START.
    //In case the DAQ gets STOP from the WEB DCS, the DAQ exits safely.
    //In case of an error, the DAQ sends STATUS_ERR or WRITE_ERR and exits.

    /* Initialisation of the setup */

    MSG_INFO("[DAQ] Initialisation of the TDCs. Please wait, this may take a few minutes...");

    DR->Init(__configpath);

    MSG_INFO("[DAQ] Initialisation done");

    //When the initialisation is done, send to the WEBDCS a DAQ_RDY signal
    SendDAQReady();

    /* Run */

    //Enter the DAQ Loop only if we got a first START
    //If the DAQ was ready before the ramping, wait until START
    WaitDCSSignal(2);

    if(CheckSTART()){

        //Stay in the run loop while you don't have STOP
        while(!CheckSTOP()){
            //Start the run after having updated the configuration file
            MSG_INFO("[DAQ] Run about to start...");

            SendDAQRunning();
            DR->Update();
            DR->Run();

            MSG_INFO("[DAQ] Run finished. Waiting for the next signal...");

            //When run is done, send a DAQ_RDY signal
            SendDAQReady();

            //Wait for the WEB DCS to send a new signal and control
            //that it's START or STOP
            WaitDCSSignal(2);

            if(!CheckSTART() && !CheckSTOP()){
                MSG_ERROR("[DAQ-ERROR] Wrong DCS signal received");
                MSG_ERROR("[DAQ-ERROR] DAQ will shut down");
                SendDAQError();
                exit(EXIT_FAILURE);
            }
        }
    }

    MSG_INFO("[DAQ-STOP] DAQ will shut down");
    return 0;
}

