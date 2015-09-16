// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************
#ifndef __LINUX
#define __LINUX
#endif

#ifndef DATAREADER_H
#define DATAREADER_H

#include <string>

#include "v1190a.h"
#include "v1718.h"

using namespace std;

class DataReader
{
    private:
        bool                StopFlag;
        IniFile            *iniFile;
        Data32              MaxTriggers;
        v1718              *VME;
        int                 nTDCs;
        v1190a             *TDCs;
        RAWData             TDCData;

    public:
        DataReader();
        virtual            ~DataReader();
        void                SetIniFile(string inifilename);
        void                SetMaxTriggers();
        Data32              GetMaxTriggers();
        void                SetVME();
        void                SetTDC();
        void                Init(string inifilename);
        string              GetRunNumber(string runregistry);
        string              GetFileName(string runregistry);
        void                Run();
};

#endif // DATAREADER_H
