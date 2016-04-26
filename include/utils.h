#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "../include/CAENVMEtypes.h"

using namespace std;

#define RUNNING     3
#define DAQ_RDY     2
#define START       1
#define STOP       -1
#define STATUS_ERR -2

#define WR_OK       4
#define WR_ERR     -4
#define RD_ERR     -5

const string __nfspath          = "/mnt/nfs/daq_data/";
const string __datapath         = __nfspath + "DAQ/";
const string __logpath          = __nfspath + "DAQ_RUN/log";
const string __runstatuspath    = __nfspath + "DAQ_RUN/run";
const string __configpath       = __nfspath + "DAQ_RUN/daqgifpp.ini";
const string __parampath        = __nfspath + "DAQ_RUN/mon";
const string __registrypath     = __nfspath + "DAQ_RUN/runregistry";

string      shortTostring(short value);
string      intTostring(int value);
string      UintTostring(Uint value);
string      longTostring(long value);
string      longlongTostring(long long value);
string      floatTostring(float value);
string      GetRunStatus();
int         SetRunStatus(string& runStatus);
int         CtrlRunStatus(string& runStatus);
void        CheckKILL();
long long   GetTimeStamp();
string      GetLogTimeStamp();

#endif // UTILS_H
