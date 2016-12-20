#ifndef _MSGSVC_CXX_
#define _MSGSVC_CXX_
//***************************************************************
// *   MsgSvc.h
// *   Alexis Fagot
// *   22/04/2016
//***************************************************************

#include "../include/MsgSvc.h"
#include "../include/utils.h"
#include <iostream>
#include <fstream>

using namespace std;

int MSG(string message, int level){
    //First we need to get the log file path into the log life
    //in the RUN directory, then we will know where to write the
    //logs.
    string logpath;

    ifstream logpathfile(__logpath.c_str(), ios::in);
    logpathfile >> logpath;
    logpathfile.close();

    ofstream logfile(logpath.c_str(), ios::app);

    if(logfile){
        logfile << GetLogTimeStamp() << message << endl;
        logfile.close();
        return level;
    } else
        return WR_ERR;
}

void MSG_FATAL(string message)  {MSG(message,FATAL);}
void MSG_ERROR(string message)  {MSG(message,ERROR);}
void MSG_WARNING(string message){MSG(message,WARNING);}
void MSG_INFO(string message)   {MSG(message,INFO);}
void MSG_DEBUG(string message)  {MSG(message,DEBUG);}
void MSG_VERBOSE(string message){MSG(message,VERBOSE);}
void MSG_ALWAYS(string message) {MSG(message,ALWAYS);}

void PrintLogV1190(int tdc, string parameter, Data16 value){
    string tdcnumber = intTostring(tdc);
    string readregister = intTostring(value);
    MSG_INFO("[TDC"+tdcnumber+"] "+parameter+" : "+readregister);
}


#endif
