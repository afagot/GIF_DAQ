#ifndef _MSGSVC_H_
#define _MSGSVC_H_
//***************************************************************
// *   MsgSvc.h
// *   Alexis Fagot
// *   22/04/2016
//***************************************************************

#include "../include/utils.h"
#include <string>

using namespace std;

#define FATAL   -3
#define ERROR   -2
#define WARNING -1
#define INFO     0
#define DEBUG    1
#define VERBOSE  2
#define ALWAYS   3


int MSG(string message, int level);

void MSG_FATAL   (string message);
void MSG_ERROR   (string message);
void MSG_WARNING (string message);
void MSG_INFO    (string message);
void MSG_DEBUG   (string message);
void MSG_VERBOSE (string message);
void MSG_ALWAYS  (string message);

void PrintLogV1190(int tdc, string parameter, string readregister);

#endif

