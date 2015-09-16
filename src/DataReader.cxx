// ****************************************************************************************************
// *   DataReader
// *   Alexis Fagot
// *   23/01/2015
// ****************************************************************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>

#include "../include/DataReader.h"
#include "../include/MsgSvc.h"

using namespace std;

// ****************************************************************************************************

DataReader::DataReader(){
    //Initialisation of the RAWData vectors
    TDCData.EventList = new vector<int>;
    TDCData.NHitsList = new vector<int>;
    TDCData.ChannelList = new vector< vector<int> >;
    TDCData.TimeStampList = new vector< vector<float> >;

    //Cleaning all the vectors
    TDCData.EventList->clear();
    TDCData.NHitsList->clear();
    TDCData.ChannelList->clear();
    TDCData.TimeStampList->clear();

    StopFlag = false;
}

// ****************************************************************************************************

DataReader::~DataReader(){

}

// ****************************************************************************************************

void DataReader::SetIniFile(string inifilename){
    iniFile = new IniFile(inifilename);
    iniFile->Read();
}

// ****************************************************************************************************

void DataReader::SetMaxTriggers(){
    MaxTriggers = iniFile->intType("General","MaxTriggers",MAXTRIGGERS_V1190A);
}

// ****************************************************************************************************

Data32 DataReader::GetMaxTriggers(){
    return MaxTriggers;
}

// ****************************************************************************************************

void DataReader::SetVME(){
    VME = new v1718(iniFile);
}

// ****************************************************************************************************

void DataReader::SetTDC(){
    nTDCs = iniFile->intType("General","Tdcs",MINNTDC);
    TDCs = new v1190a(VME->GetHandle(),iniFile,nTDCs);

    /*********** initialize the TDC 1190a ***************************/
    TDCs->Set(iniFile,VME,nTDCs);
}

// ****************************************************************************************************

void DataReader::Init(string inifilename){
    SetIniFile(inifilename);
    SetMaxTriggers();
    SetVME();
    SetTDC();
}

// ****************************************************************************************************

string DataReader::GetRunNumber(string runregistry){
    //Get the date first and then write it in the run registry along with the run number
    stringstream stream;

    //Get the date
    time_t t = time(0);
    struct tm *Time = localtime(&t);
    int Y = Time->tm_year + 1900;
    int M = Time->tm_mon + 1;
    int D = Time->tm_mday;
    int h = Time->tm_hour;
    int m = Time->tm_min;
    int s = Time->tm_sec;

    string Date;

    stream << setfill('0') << setw(4) << Y << "/"
           << setfill('0') << setw(2) << M << "/"
           << setfill('0') << setw(2) << D << "-"
           << setfill('0') << setw(2) << h << ":"
           << setfill('0') << setw(2) << m << ":"
           << setfill('0') << setw(2) << s;

    stream >> Date;
    stream.clear();

    //Check if the run registry file already exists or not
    //After the first usage of the DAQ, this is also used to
    //control the good reading of the file.
    ifstream checkifexits(runregistry.c_str(),ios::in);

    //Get the run number
    string RunName;
    Uint runnumber = 0;

    if(!checkifexits){
        MSG_INFO("No registry file found. Creating the file %s and initialising to run 0\n",runregistry.c_str());
    } else {
        ifstream runregRead(runregistry.c_str(),ios::in);

        Uint lastrun;
        string lastdate;

        while(runregRead.good()){
            runregRead >> lastrun >> lastdate;
        }

        //increment the run number to get the new run number
        runnumber = lastrun + 1;

        MSG_INFO("Adding run %06u the file %s\n",runnumber,runregistry.c_str());
    }

    stream << setfill('0') << setw(6) << runnumber;
    stream >> RunName;
    stream.clear();

    //Write this new run number in the run registry
    ofstream runregWrite(runregistry.c_str(),ios::app);
    runregWrite << RunName << '\t' << Date;

    return RunName;
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

    stringstream fNameStream;
    fNameStream << "datarun/run"                                    //destination + filename prefix "run"
                << GetRunNumber(".RunRegistry/RunRegistry.csv");    //run number
    for(int i=0; i<9;i++)
        fNameStream << fNameParts[i];                               //run info
    fNameStream << ".root";

    string outputfName;
    fNameStream >> outputfName;

    return outputfName;
}

// ****************************************************************************************************

void DataReader::Run(){
    MSG_INFO("[DAQ]: Starting data acquisition\n");
    MSG_INFO("[DAQ]: %d triggers will be taken\n", GetMaxTriggers());

    Uint TriggerCount = 0;
    string outputFileName = GetFileName();

    TFile *outputFile = new TFile(outputFileName.c_str(), "recreate");
    TTree *RAWDataTree = new TTree("RAWData","RAWData");

    int               EventCount = -9;
    int               nHits = -8;
    vector<int>       TDCCh;
    vector<float>     TDCTS;

    TDCCh.clear();
    TDCTS.clear();

    RAWDataTree->Branch("EventNumber",    &EventCount,  "EventNumber/I");
    RAWDataTree->Branch("number_of_hits", &nHits,       "number_of_hits/I");
    RAWDataTree->Branch("TDC_channel",    &TDCCh);
    RAWDataTree->Branch("TDC_TimeStamp",  &TDCTS);

    //Read the output buffer until the min number of trigger is achieved
    while(TriggerCount < GetMaxTriggers()){
        usleep(200000);

        if(VME->CheckIRQ()){
            //Stop data acquisition with BUSY as VETO
            VME->SendBUSY(ON);
            usleep(1000);

            //Read the data
            TriggerCount = TDCs->Read(&TDCData,nTDCs);
            if(TriggerCount != 0) MSG_INFO("\n[DAQ]: %d / %d taken\n", TriggerCount, GetMaxTriggers());
            else MSG_INFO(".");

            //Resume data taking
            VME->SendBUSY(OFF);
        } else MSG_INFO(".");
    }

    //Write the data from the RAWData sstructure to the TTree
    for(Uint i=0; i<TDCData.EventList->size(); i++){
        EventCount  = TDCData.EventList->at(i);
        nHits       = TDCData.NHitsList->at(i);
        TDCCh       = TDCData.ChannelList->at(i);
        TDCTS       = TDCData.TimeStampList->at(i);

        RAWDataTree->Fill();
    }

    RAWDataTree->Print();
    outputFile = RAWDataTree->GetCurrentFile();
    outputFile->Write(0, TObject::kWriteDelete);

    delete outputFile;
}
