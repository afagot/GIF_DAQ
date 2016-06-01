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
        Data32              nTriggers[2];
        v1718              *VME;
        int                 nTDCs;
        v1190a             *TDCs;
        RAWData             TDCData;
        TriggerType         currentTrigger;

    public:
        DataReader();
        virtual            ~DataReader();
        void                SetIniFile(string inifilename);
        void                SetNtriggers();
        Data32              GetNtriggers(TriggerType whichTrigger);
        bool                GotEnoughEvent(Data32 ntriggers[]);
        void                SetVME();
        TriggerType         WhichTrigger();
        void                SetCurrentTrigger();
        bool                HasTriggerChanged();
        void                SetTDC();
        void                Init(string inifilename);
        void                Update();
        void                FlushBuffer();
        string              GetFileName();
        void                PrintPercentage(string fName, Uint current[], Uint last[]);
        void                WriteRunRegistry(string filename);
        void                Run();
};

#endif // DATAREADER_H
