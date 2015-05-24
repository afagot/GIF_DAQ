// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************
#ifndef __LINUX
#define __LINUX
#endif

#include <string>

#include "v1190a.h"
#include "v1718.h"

using namespace std;

#ifndef DATAREADER_H
#define DATAREADER_H

class DataReader
{
    private:
        bool            StopFlag;
        IniFile        *iniFile;
        Data32          MaxTriggers;
        v1718          *VME;
        v1190a         *TDC;

    public:
        DataReader();
        virtual ~DataReader();
        void            SetIniFile(string inifilename);
        void            SetMaxTriggers();
        Data32          GetMaxTriggers();
        void            SetVME();
        void            SetTDC();
        void            Init(string inifilename);
        string          GetFileName();
        void            Run();
};

#endif // DATAREADER_H
