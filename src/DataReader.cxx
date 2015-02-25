// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************

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

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"
#include "../include/CAENVMEtypes.h"

#include "../include/v1190a.h"
#include "../include/v1718.h"
#include "../include/DataReader.h"
#include "../include/IniFile.h"

using namespace std;

// ****************************************************************************************************

DataReader::DataReader()
{
    this->StopFlag = false;
}

// ****************************************************************************************************

DataReader::~DataReader()
{

}

// ****************************************************************************************************

void DataReader::SetIniFile(string inifilename)
{
    this->iniFile = new IniFile(inifilename);
    this->iniFile->Read();
}

// ****************************************************************************************************

void DataReader::SetMaxTriggers()
{
    this->MaxTriggers = this->iniFile->intType("General","MaxTriggers",MAXTRIGGERS_V1190A);
}

// ****************************************************************************************************

Data32 DataReader::GetMaxTriggers()
{
    return this->MaxTriggers;
}

// ****************************************************************************************************

void DataReader::SetVME()
{
    this->VME = new v1718(this->iniFile);
}

// ****************************************************************************************************

void DataReader::SetTDC()
{
    this->TDC = new v1190a(this->VME->GetHandle(),this->iniFile);

    /*********** initialize the TDC 1190a ***************************/
    this->TDC->Set(this->iniFile);
}

// ****************************************************************************************************

void DataReader::Init(string inifilename)
{
    this->SetIniFile(inifilename);
    this->SetMaxTriggers();
    this->SetVME();
    this->SetTDC();
}

// ****************************************************************************************************

void DataReader::Run()
{
    cout << "Starting data acquisition." << endl;

    Uint triggercount = 0;

    while(triggercount < this->GetMaxTriggers()){
        triggercount += this->TDC->Read(this->VME);
    }
}
