// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>

#include "../include/DataReader.h"
#include "../include/MsgSvc.h"

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

void DataReader::SetIniFile(string inifilename)
{
    iniFile = new IniFile(inifilename);
    iniFile->Read();
}

// ****************************************************************************************************

void DataReader::SetMaxTriggers()
{
    MaxTriggers = iniFile->intType("General","MaxTriggers",MAXTRIGGERS_V1190A);
}

// ****************************************************************************************************

Data32 DataReader::GetMaxTriggers()
{
    return MaxTriggers;
}

// ****************************************************************************************************

void DataReader::SetVME()
{
    VME = new v1718(iniFile);
}

// ****************************************************************************************************

void DataReader::SetTDC()
{
    TDC = new v1190a(VME->GetHandle(),iniFile);

    /*********** initialize the TDC 1190a ***************************/
    TDC->Set(iniFile);
}

// ****************************************************************************************************

void DataReader::Init(string inifilename)
{
    SetIniFile(inifilename);
    SetMaxTriggers();
    SetVME();
    SetTDC();
}

// ****************************************************************************************************

string DataReader::GetFileName(){
    string fNameParts[9];
    fNameParts[0] = iniFile->stringType("General","RunType","");
    fNameParts[1] = iniFile->stringType("General","ChamberType","");
    fNameParts[2] = iniFile->stringType("General","Mode","");
    fNameParts[3] = iniFile->stringType("General","Partition","");
    fNameParts[4] = iniFile->stringType("General","MaxTrigger","");
    fNameParts[5] = iniFile->stringType("General","TriggerType","");
    fNameParts[6] = iniFile->stringType("General","ElectronicsType","");
    fNameParts[7] = iniFile->stringType("General","Threshold","");
    fNameParts[8] = iniFile->stringType("General","Voltage","");

    for(int i=0; i<9;i++)
        if(fNameParts[i] != "")
            fNameParts[i] += "_";

    time_t t = time(0);
    struct tm *Time = localtime(&t);
    int Y = Time->tm_year + 1900;
    int M = Time->tm_mon + 1;
    int D = Time->tm_mday;
    int h = Time->tm_hour;
    int m = Time->tm_min;
    int s = Time->tm_sec;

    stringstream fNameStream;
    fNameStream << "datarun/";                      //destination
    for(int i=0; i<9;i++)
        fNameStream << fNameParts[i];               //informations about chamber, trigger and electronics
    fNameStream << "run"
                << setfill('0') << setw(4) << Y     //run number
                << setfill('0') << setw(2) << M
                << setfill('0') << setw(2) << D
                << setfill('0') << setw(2) << h
                << setfill('0') << setw(2) << m
                << setfill('0') << setw(2) << s
                << ".root";
                //<< ".dat";                          //file type

    string outputfName;
    fNameStream >> outputfName;

    return outputfName;
}

// ****************************************************************************************************

void DataReader::Run()
{
    MSG_INFO("Starting data acquisition\n");
    MSG_INFO("%d triggers will be taken\n", GetMaxTriggers());

    Uint TriggerCount = 0;
    string outputFileName = GetFileName();

    TFile *outputFile = new TFile(outputFileName.c_str(), "recreate");
    TTree *RAWDataTree = new TTree("RAWData","RAWData");

    int               EventCount = -9;
    int               nHits       = -8;
    vector<int>      *TDCCh       = new vector<int>;
    vector<float>    *TDCTS       = new vector<float>;

    RAWDataTree->Branch("EventNumber",    &EventCount,   "EventNumber/I");
    RAWDataTree->Branch("number_of_hits", &nHits,         "number_of_hits/I");
    RAWDataTree->Branch("TDC_channel",    &TDCCh);
    RAWDataTree->Branch("TDC_TimeStamp",  &TDCTS);

    while(TriggerCount < GetMaxTriggers()){
        usleep(20000);

        Uint lastCount = TriggerCount;
        //if(VME->CheckIRQ()) TriggerCount += TDC->Read(outputFileName);
        if(VME->CheckIRQ()) TriggerCount += TDC->Read(RAWDataTree,EventCount,nHits,TDCCh,TDCTS);

        if(TriggerCount != lastCount) MSG_INFO("%d / %d taken\n", TriggerCount, GetMaxTriggers());
    }

    RAWDataTree->Print();
    outputFile = RAWDataTree->GetCurrentFile();
    outputFile->Write(0, TObject::kWriteDelete);

    delete outputFile;
}
