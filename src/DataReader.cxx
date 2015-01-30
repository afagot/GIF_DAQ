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
    StopFlag = false;
}

// ****************************************************************************************************

DataReader::~DataReader()
{

}

// ****************************************************************************************************

void DataReader::Init(string inifilename)
{
    IniFile* inifile = new IniFile(inifilename);
    inifile->Read();

    MaxSpills = inifile->Long("General","MaxSpills",MAXSPILLS_V1190A);

    VME = new v1718(inifile);
    TDC = new v1190a(VME->GetHandle(),inifile);

    /*********** initialize the TDC 1190a ***************************/
    TDC->Set(inifile);
}

// ****************************************************************************************************

void DataReader::Run()
{
    cout << "Starting data acquisition." << endl;

    Uint spillcount = 0;

    while(spillcount < MaxSpills){
        spillcount += TDC->Read(this->VME);
    }
}
