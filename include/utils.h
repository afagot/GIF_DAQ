#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "utils.h"

using namespace std;

#define PROCESSING  4
#define RUNNING     3
#define DAQ_RDY     2
#define START       1
#define INIT        0
#define STOP       -1
#define STATUS_ERR -2
#define DAQ_ERR    -3
#define ARGC_ERR   -4

#define WR_OK       5
#define WR_ERR     -5
#define RD_ERR     -6

const string __nfspath       = "/var/operation/";
const string __datapath      = __nfspath + "HVSCAN/";
const string __runpath       = __nfspath + "RUN/";
const string __logpath       = __runpath + "log-daq";
const string __runstatuspath = __runpath + "run";
const string __configpath    = __runpath + "daqgifpp.ini";
const string __registrypath  = __runpath + "runregistry";
const string __lastfilepath  = __runpath + "last";

/*** Types ***/

typedef unsigned char Uchar;
typedef Uchar  Data8;

typedef unsigned short Ushort;
typedef Ushort Data16;
typedef Ushort Word16;

typedef unsigned int Uint;
typedef Uint   Data32;
typedef Uint   Word32;

/*** Functions ***/

string    shortTostring(short value);
string    intTostring(int value);
string    UintTostring(Uint value);
string    longTostring(long value);
string    longlongTostring(long long value);
string    floatTostring(float value);
string    GetRunStatus();
int       SetRunStatus(string& runStatus);
int       CtrlRunStatus(string& runStatus);
void      SendDAQReady();
void      SendDAQProcess();
void      SendDAQRunning();
void      SendDAQError();
void      WaitDCSSignal(Uint delay);
void      CheckKILL();
bool      CheckSTART();
bool      CheckSTOP();
long long GetTimeStamp();
string    GetLogTimeStamp();

#endif // UTILS_H
