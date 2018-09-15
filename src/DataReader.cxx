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

#include "TH1I.h"
#include "TH1D.h"

#include "../include/DataReader.h"
#include "../include/MsgSvc.h"
#include "../include/utils.h"

using namespace std;

// ****************************************************************************************************

DataReader::DataReader(){
    //Initialisation of the RAWData vectors
    TDCData.EventList = new vector<int>;
    TDCData.NHitsList = new vector<int>;
    TDCData.QFlagList = new vector<int>;
    TDCData.ChannelList = new vector< vector<int> >;
    TDCData.TimeStampList = new vector< vector<float> >;

    //Cleaning all the vectors
    TDCData.EventList->clear();
    TDCData.NHitsList->clear();
    TDCData.QFlagList->clear();
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
    TDCs->Set(iniFile,nTDCs);
}

// ****************************************************************************************************

int DataReader::GetQFlag(Uint it){
    int flag = TDCData.QFlagList->at(it);
    int nDigits = nTDCs;

    int tmpflag = flag;
    while(nDigits != 0){
        int tdcflag = tmpflag/(int)pow(10,nDigits-1);

        if(tdcflag == CORRUPTED) flag = flag + 2*(int)pow(10,nDigits-1);

        tmpflag = tmpflag%(int)pow(10,nDigits-1);
        nDigits--;
    }

    return flag;
}

// ****************************************************************************************************

void DataReader::Init(string inifilename){
    SetIniFile(inifilename);
    SetMaxTriggers();
    SetVME();
    SetTDC();
}

// ****************************************************************************************************

void DataReader::Update(){
    iniFile->Read();
    SetMaxTriggers();
}

// ****************************************************************************************************

void DataReader::FlushBuffer(){
    VME->SendBUSY(ON);
    TDCs->Clear(nTDCs);
    VME->SendBUSY(OFF);
}

// ****************************************************************************************************

string DataReader::GetFileName(){
    //Get the scan number from the ini file
    int ScanNumber = iniFile->intType("General","ScanID",999999);

    //Create the folder to contain the data file is it doesn't exist yet
    //We will use a convention of 6 digits for the Scan ID
    stringstream ss;
    ss << setfill('0') << setw(6) << ScanNumber;
    string ScanID;
    ss >> ScanID;

    string datafolder = __datapath + ScanID + "/";
    string mkdirScanFolder = "mkdir -p " + datafolder;
    system(mkdirScanFolder.c_str());

    //Get the HV step
    string HVstep = iniFile->stringType("General","HV","HVX");

    //use a stream to construct the name with the different variable types
    stringstream fNameStream;

    fNameStream << datafolder   //destination
                << "Scan"
                << ScanID       //scan ID
                << "_HV"
                << HVstep       //HV step
                << "_DAQ.root"; //extension

    string outputfName;
    fNameStream >> outputfName;

    return outputfName;
}

// ****************************************************************************************************

void DataReader::WriteRunRegistry(string filename){
    //Open the run registry file and wirte the new file name
    ofstream runregistry(__registrypath.c_str(),ios::app);

    string filepath = filename.substr(0,filename.find_last_of("/")+1);
    string name = filename.substr(filename.find_last_of("/")+1);

    runregistry << GetTimeStamp() << '\t' << name << '\t' << filepath << '\n';
    runregistry.close();

    //Save the last run name in the "last" file (useful to start
    //offline analysis from DCS)
    ofstream last(__lastfilepath.c_str(),ios::out);
    last << name.substr(0,name.find_last_of("."));
    last.close();
}

// ****************************************************************************************************

void DataReader::Run(){
    //Get the output file name and create the ROOT file
    Uint TriggerCount = 0;
    string outputFileName = GetFileName();
    long long startstamp = GetTimeStamp();

    TFile *outputFile = new TFile(outputFileName.c_str(), "recreate");

    //Create the data tree where the data will be saved
    //For each entry will be saved the event tag, the number of hits recorded
    //in the TDCs, the list of fired TDC channels and the time stamps of the
    //hits.
    TTree *RAWDataTree = new TTree("RAWData","RAWData");

    int           EventCount = -9;  //Event tag
    int           nHits = -8;       //Number of fired TDC channels in event
    int           qflag = -7;       //Event quality flag (0 = CORRUPTED | 1 = GOOD)
    vector<int>   TDCCh;            //List of fired TDC channels in event
    vector<float> TDCTS;            //list of fired TDC channels time stamps

    TDCCh.clear();
    TDCTS.clear();

    //Set the branches that will contain the previously defined variables
    RAWDataTree->Branch("EventNumber",    &EventCount, "EventNumber/I");
    RAWDataTree->Branch("number_of_hits", &nHits,      "number_of_hits/I");
    RAWDataTree->Branch("Quality_flag",   &qflag,      "Quality_flag/I");
    RAWDataTree->Branch("TDC_channel",    &TDCCh);
    RAWDataTree->Branch("TDC_TimeStamp",  &TDCTS);

    //Cleaning all the vectors that will contain the data
    TDCData.EventList->clear();
    TDCData.NHitsList->clear();
    TDCData.QFlagList->clear();
    TDCData.ChannelList->clear();
    TDCData.TimeStampList->clear();

    //Clear all the buffers that can have started to be filled
    //by incoming triggers and start data taking. If non efficiency
    //tun type, turn ON trigger pulse.
    FlushBuffer();
    TString runtype = iniFile->stringType("General","RunType","");
    if(runtype == "rate" || runtype == "noise_reference" || runtype == "test"){
        VME->RDMTriggerPulse(ON);
        MSG_INFO("[DAQ] Run is of type "+(string)runtype+" - starting random trigger pulses");
    }

    MSG_INFO("[DAQ] Run "+outputFileName+" started");
    MSG_INFO("[DAQ] Run "+outputFileName+" 0%");

    Uint percentage = 0;     // percentage of the run done
    Uint last_print = 0;     // keep track of the last percentage printed

    //Every once in a while read the run file to check for a KILL command
    //Create a check kill clock
    Uint CKill_Clk = 0;
    Uint CKill_Clk_Cycle = (Uint)CHECKKILLPERIOD/(CHECKIRQPERIOD*1e-6);
    bool KillReceived = false;

    //Read the output buffer until the min number of trigger is achieved
    while(TriggerCount < GetMaxTriggers() && !KillReceived){
        //Check the TDC buffers for data every 100ms
        //If there is data, an interupt request is present
        usleep(CHECKIRQPERIOD);

        if(VME->CheckIRQ()){
            //Stop data acquisition with BUSY as VETO (the rising time of
            //the signal is of the order of 1ms)
            VME->SendBUSY(ON);
            usleep(1000);

            //Read the data
            TriggerCount = TDCs->Read(&TDCData,nTDCs);

            //percentage update
            percentage = (100*TriggerCount) / GetMaxTriggers();

            //dump the status in the logfile every 5%
            if(percentage != 0 && percentage % 5 == 0 && percentage != last_print){
                string log_percent = intTostring(percentage);

                MSG_INFO("[DAQ] Run "+outputFileName+" "+log_percent+"%");
                last_print = percentage;
            }

            //Resume data taking - Release VETO signal
            VME->SendBUSY(OFF);
        }

        //Increment the kill clock
        CKill_Clk++;

        //check inside the run file for a KILL command
        if(CKill_Clk == CKill_Clk_Cycle){
            KillReceived = CheckKILL();
            CKill_Clk = 0;
        }
    }

    //Stop random trigger pulses if non efficiency run type
    if(runtype == "rate" || runtype == "noise_reference" || runtype == "test"){
        VME->RDMTriggerPulse(OFF);
        MSG_INFO("[DAQ] Stopping random trigger pulses");
    }

    //Write the data from the RAWData structure to the TTree and
    //change the QFlag digits that are equal to 0, to 2 for later
    //offline analysis.
    for(Uint i=0; i<TDCData.EventList->size(); i++){
        EventCount  = TDCData.EventList->at(i);
        nHits       = TDCData.NHitsList->at(i);
        qflag       = GetQFlag(i);
        TDCCh       = TDCData.ChannelList->at(i);
        TDCTS       = TDCData.TimeStampList->at(i);

        RAWDataTree->Fill();
    }

    RAWDataTree->Print();
    RAWDataTree->Write();

    delete RAWDataTree;

    //******************************************************************************

    //At the end of each run, the HVeff, Threshold, mean HVmon, Imon and environmental
    //parameters values, beam and source status, Attenuator settings, Run type,
    //Scan ID, Number of triggers, start and stop time stamps are saved

    //----------------------- CONFIGURATION FILE DATA

    //Create the parameters TTree to save the string parameters
    TTree *RunParameters = new TTree("RunParameters","RunParameters");

    //String parameters are to be read from the config file and saved into
    //the TTree
    TString beamstatus;     //Beam status (ON or OFF)

    //Branches linking the string variables to the TTree
    RunParameters->Branch("RunType", &runtype);
    RunParameters->Branch("Beam", &beamstatus);

    //Int parameters are to be read from the config file and saved into
    //histograms
    TH1D *ID   = new TH1D("ID","Identifiers of this run",4,0,4); //To save Scan ID, HV step, Start and Stop time stamps
    ID->SetOption("TEXT");
    TH1I *Trig = new TH1I("Triggers","Number of triggers for this run",1,0,1); //Number of triggers
    Trig->SetOption("TEXT");
    TH1I *Thrs = new TH1I("Thrs","List of thresholds used per chamber during this run",1,0,1); //List of Thresholds
    Thrs->SetOption("TEXT");
    Thrs->SetCanExtend(TH1::kAllAxes); //Since the number of chambers can change, I chose a dynamical axis

    //Needed variable to go through the configuration file
    string group;
    string Parameter;
    string RPClabel;
    int value = 0;


    //Now fill the configuration parameters
    IniFileData inidata = iniFile->GetFileData();

    for(IniFileDataIter Iter = inidata.begin(); Iter!= inidata.end(); Iter++){
        size_t separator_pos = Iter->first.find_first_of('.');
        if (separator_pos == string::npos)
            continue;

        group = Iter->first.substr(0, separator_pos);

        //Parameters to be found in the [General] group
        if(group == "General"){
            Parameter = Iter->first.substr(separator_pos+1);
            if(Parameter == "ScanID"){
                value = iniFile->intType(group,Parameter,0);
                ID->Fill(Parameter.c_str(), value);
                ID->Fill("Start stamp", startstamp);
                ID->Fill("Stop stamp", GetTimeStamp());
            } else if (Parameter == "HV"){
                value = iniFile->intType(group,Parameter,0);
                ID->Fill(Parameter.c_str(), value);
            } else if(Parameter == "MaxTriggers"){
                value = iniFile->intType(group,Parameter,0);
                Trig->Fill(Parameter.c_str(),value);
            } else if(Parameter == "Beam"){
                beamstatus = iniFile->stringType(group,Parameter,"");
            }
        }

        //Parameters to be found in the [Threshold] group
        else if (group == "Threshold"){
            RPClabel = Iter->first.substr(separator_pos+1);
            value = iniFile->intType(group,RPClabel,0);
            Thrs->Fill(RPClabel.c_str(),value);
        }
    }

    //Write the histograms into the ROOT file
    ID->Write();
    Trig->Write();
    Thrs->Write();

    //Fill the parameters tree, print it and write it into the ROOT file
    RunParameters->Fill();
    RunParameters->Print();
    RunParameters->Write();

    outputFile->Close();

    //If the data taking went well, add the data file to the run registry
    WriteRunRegistry(outputFileName);

    delete outputFile;

    //Finally give the permission to the DCS to delete the file if necessary
    string GivePermission = "chmod 775 " + outputFileName;
    system(GivePermission.c_str());
}
