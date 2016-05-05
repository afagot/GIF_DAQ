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
    TDCs->Set(iniFile,nTDCs);
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

    //Get the run number (start time of the run)
    long long RunNumber = GetTimeStamp();

    //use a stream to construct the name with the different variable types
    stringstream fNameStream;

    fNameStream << datafolder   //destination
                << "Scan"
                << ScanID       //scan ID
                << "_Run"
                << RunNumber    //run number
                << ".root";     //extension

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

    int               EventCount = -9;  //Event tag
    int               nHits = -8;       //Number of fired TDC channels in event
    vector<int>       TDCCh;            //List of fired TDC channels in event
    vector<float>     TDCTS;            //list of fired TDC channels time stamps

    TDCCh.clear();
    TDCTS.clear();

    //Set the branches that will contain the previously defined variables
    RAWDataTree->Branch("EventNumber",    &EventCount,  "EventNumber/I");
    RAWDataTree->Branch("number_of_hits", &nHits,       "number_of_hits/I");
    RAWDataTree->Branch("TDC_channel",    &TDCCh);
    RAWDataTree->Branch("TDC_TimeStamp",  &TDCTS);

    //Cleaning all the vectors that will contain the data
    TDCData.EventList->clear();
    TDCData.NHitsList->clear();
    TDCData.ChannelList->clear();
    TDCData.TimeStampList->clear();

    //Clear all the buffers and start data taking
    VME->SendBUSY(ON);
    TDCs->Clear(nTDCs);
    VME->SendBUSY(OFF);

    //Print log message
    MSG_INFO("[DAQ] Run "+outputFileName+" started");
    MSG_INFO("[DAQ] Run "+outputFileName+" 0%");

    Uint percentage = 0;     // percentage of the run done
    Uint last_print = 0;     // keep track of the last percentage printed

    //Every once in a while read the run file to check for a KILL command
    //Create a check kill clock
    Uint CKill_Clk = 0;

    //Read the output buffer until the min number of trigger is achieved
    while(TriggerCount < GetMaxTriggers()){
        //Check the TDC buffers for data every .1s (100ms)
        usleep(100000);

        if(VME->CheckIRQ()){
            //Stop data acquisition with BUSY as VETO (the rising of
            //the signal is of the order of 1ms)
            VME->SendBUSY(ON);
            usleep(1000);

            //Read the data
            TriggerCount = TDCs->Read(&TDCData,nTDCs);

            //percentage update
            percentage = (100*TriggerCount) / GetMaxTriggers();

            //dump the status in the logfile every 5%
            if(percentage % 5 == 0 && percentage != last_print){
                string log_percent = intTostring(percentage);

                MSG_INFO("[DAQ] Run "+outputFileName+" "+log_percent+"%");
                last_print = percentage;
            }

            //Resume data taking
            VME->SendBUSY(OFF);
        }

        //Increment the kill clock
        CKill_Clk++;

        //check inside the run file for a KILL command every 10s
        if(CKill_Clk == 50){
            CheckKILL();
            CKill_Clk = 0;
        }
    }

    MSG_INFO("[DAQ] Run "+outputFileName+" 100%");

    //Write the data from the RAWData structure to the TTree
    for(Uint i=0; i<TDCData.EventList->size(); i++){
        EventCount  = TDCData.EventList->at(i);
        nHits       = TDCData.NHitsList->at(i);
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
    TString runtype;        //Type of run (Efficiency, rate or test)
    TString beamstatus;     //Beam status (ON or OFF)
    TString sourcestatus;   //Source Status (ON or OFF)
    TString electronics;    //Electronics used (for now only CMS-FEB)

    //Branches linking the string variables to the TTree
    RunParameters->Branch("RunType",            &runtype);
    RunParameters->Branch("Beam",               &beamstatus);
    RunParameters->Branch("Source",             &sourcestatus);
    RunParameters->Branch("ElectronicsType",    &electronics);

    //Int parameters are to be read from the config file and saved into
    //histograms
    TH1D *ID        = new TH1D("ID","Identifiers of this run",3,0,3); //To save Scan ID, Start and Stop time stamps
    TH1I *Att       = new TH1I("Attenuators","Attenuators settings for this run",2,0,2); //Attenuators used
    TH1I *Trig      = new TH1I("Triggers","Number of triggers for this run",1,0,1); //Number of triggers
    TH1I *Thrs      = new TH1I("Thrs","List of thresholds used per chamber during this run",1,0,1); //List of Thresholds
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
                ID->Fill(Parameter.c_str(),value);
                ID->Fill("Start stamp",startstamp);
                ID->Fill("Stop stamp", GetTimeStamp());
            } else if(Parameter == "RunType"){
                runtype = iniFile->stringType(group,Parameter,"");
            } else if(Parameter == "MaxTriggers"){
                value = iniFile->intType(group,Parameter,0);
                Trig->Fill(Parameter.c_str(),value);
            } else if(Parameter == "Beam"){
                beamstatus = iniFile->stringType(group,Parameter,"");
            } else if(Parameter == "Source"){
                sourcestatus = iniFile->stringType(group,Parameter,"");
            } else if(Parameter == "AttU" || Parameter == "AttD"){
                value = iniFile->intType(group,Parameter,0);
                Att->Fill(Parameter.c_str(),value);
            } else if(Parameter == "ElectronicsType"){
                electronics = iniFile->stringType(group,Parameter,"");
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
    Att->Write();
    Trig->Write();
    Thrs->Write();

    //Fill the parameters tree, print it and write it into the ROOT file
    RunParameters->Fill();
    RunParameters->Print();
    RunParameters->Write();

    //----------------------- MONITORING FILE DATA

    //Then make histograms for the monitored parameters
    //These parameters are continuously stored into a file every 10 seconds
    //The first line is the number of parameters saved in the file
    //The second line is the header of each column corresponding to a parameter
    //Then the rest of the file are the monitored parameters
    //The number of parameters can change from 1 run to another
    //It is needed to create a vector that will be filled with histograms for each parameters
    ifstream MonFile(__parampath.c_str(), ios::in);

    if(MonFile){
        //Send a PROCESSING signal to WEB DCS to tell it we are reading
        //into the Monitoring file
        SendDAQProcess();

        //vector that will contain the parameter histograms
        TH1D* Monitor[200];

        //read the number of monitored parameters in the file
        // (first line)
        int nParam = 0;
        MonFile >> nParam;

        //Save the names of the parameters and use them to initialise
        //a histogram that will fill the previously created vector
        // (second line)
        for(int p = 0; p < nParam; p++){
            string nameParam;
            MonFile >> nameParam;

            Monitor[p] = new TH1D(nameParam.c_str(),nameParam.c_str(),10,0,1);
            Monitor[p]->SetCanExtend(TH1::kAllAxes);
        }

        //Start the loop over the values of the monitored parameters
        double paramValue = -1.;

        while(MonFile.good()){
            for(int p = 0; p < nParam; p++){
                MonFile >> paramValue;
                if(paramValue > 0.) Monitor[p]->Fill(paramValue);

                paramValue = -1.;
            }
        }

        for(int p = 0; p < nParam; p++){
            Monitor[p]->Write();
        }
    }

    outputFile->Close();

    //If the data taking went well, add the data file to the run registry
    WriteRunRegistry(outputFileName);

    delete outputFile;
}
