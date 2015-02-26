// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************
#ifndef __LINUX
#define __LINUX
#endif

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

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "v1190a.h"
#include "v1718.h"

using namespace std;

#ifndef DATAREADER_H
#define DATAREADER_H

class DataReader
{
    private:
        bool            StopFlag;
        IniFile         *iniFile;
        Data32          MaxTriggers;
        v1718           *VME;
        v1190a          *TDC;

    public:
        DataReader();
        virtual ~DataReader();
        void SetIniFile(string inifilename);
        void SetMaxTriggers();
        Data32 GetMaxTriggers();
        void SetVME();
        void SetTDC();
        void Init(string inifilename);
        string GetFileName();
        void Run();
};

#endif // DATAREADER_H
