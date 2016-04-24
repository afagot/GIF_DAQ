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
    ofstream logfile(__logpath.c_str(), ios::app);

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


#endif
