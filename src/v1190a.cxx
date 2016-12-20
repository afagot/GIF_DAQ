// *************************************************************************************************************
// *   v1190a
// *   Alexis Fagot
// *   20/01/2015
// *   Based on v1290a file from :
// *   Avia Raviv & Y. Benhammou
// *   14/03/2012
// *
// *   Documentation about this VME TDC module can be found at the followin url
// *   http://www.caen.it/servlet/checkCaenManualFile?Id=8657
// *
// *   Comments will often refer to this user manual
// *************************************************************************************************************

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"

#include "../include/utils.h"
#include "../include/v1190a.h"
#include "../include/v1718.h"
#include "../include/MsgSvc.h"

using namespace std;

v1190a::v1190a(long handle, IniFile * inifile, int ntdcs){
    Address.clear();

    for(int tdc=0; tdc<ntdcs; tdc++){
        char groupname[10];
        sprintf(groupname,"TDC%i",tdc);
        Address.push_back(inifile->addressType(groupname,"BaseAddress",BASEV1190A)); //See page 35
    }
    DataWidth=cvD16;              //See in CAENVMEtypes.h CVDataWidth enum
    AddressModifier=cvA24_U_DATA; //See in CAENVMEtypes.h CVAddressModifier enum
    Handle=handle;
}

// *************************************************************************************************************

v1190a::~v1190a(){}

// *************************************************************************************************************

Data16 v1190a::read_op_reg(Data32 address, string error){
    //Before executing the command, check if a KILL has been sent
    CheckKILL();

    //Checks the Read OK bit
    int time = 0;
    Data16 ro_bit;

    do{
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&ro_bit,AddressModifier,DataWidth);
        time++;
    } while(ro_bit != READ_OK && time < 100000);

    if(time == 100000){
        MSG_ERROR("[v1190] Read opcode - timeout error - " + error);
        exit(0);
    } else {
        sleep(1);      /*** delay 12ms, internal delay ***/
        //Reads opcode
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_V1190A,&ro_bit,AddressModifier,DataWidth );
        return ro_bit;
    }
}

// *************************************************************************************************************

Data16 v1190a::write_op_reg(Data32 address, int code, string error){
    //Before executing the command, check if a KILL has been sent
    CheckKILL();

    //Checks the Write OK bit
    int time = 0;
    Data16 wo_bit;

    do{
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&wo_bit,AddressModifier,DataWidth );
        time++;
    } while(wo_bit != WRITE_OK && time < 100000);

    if(time == 100000){
        MSG_ERROR("[v1190-ERROR] Write opcode - timeout error - " + error);
        exit(0);
    } else {
        sleep(1);      /*** delay 12 msec, internal delay ***/
        //Writes opcode = code
        CAENVME_WriteCycle(Handle,address+ADD_MICRO_V1190A,&code,AddressModifier,DataWidth);
        return 0;
    }
}

// *************************************************************************************************************

void v1190a::Reset(int ntdcs){ //Reset the TDCs (Software clear) and wait 1s
    int reset=0x0;

    for(int tdc=0; tdc < ntdcs; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_MOD_RESET_V1190A,&reset,AddressModifier,DataWidth);

    sleep(1);
}

// *************************************************************************************************************

void v1190a::Clear(int ntdcs){ //Clear the TDC Buffers and wait 1s
    int clear=0x0;

    for(int tdc=0; tdc < ntdcs; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_SW_CLEAR_V1190A,&clear,AddressModifier,DataWidth);

    sleep(1);
}

// *************************************************************************************************************

void v1190a::TestWR(Data16 value, int ntdcs){ //Test : try to write/read 16 bit word
    for(int tdc=0; tdc < ntdcs; tdc++){
        Data16 test=value;
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

        test=0;
        CAENVME_ReadCycle(Handle,Address[tdc]+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

        if(test != value){
            string tdcnumber = intTostring(tdc);
            MSG_ERROR("[TDC"+tdcnumber+"-ERROR]  Result of W/R test is not 0xBEEF");
        }
    }
}

// *************************************************************************************************************

void v1190a::CheckTDCStatus(int ntdcs){ //Status of the TDCs (Status register)
    Data16 status;

    for(int tdc=0; tdc < ntdcs; tdc++)
        CAENVME_ReadCycle(Handle,Address[tdc]+ADD_STATUS_V1190A,&status,AddressModifier,DataWidth);
}

// *************************************************************************************************************

void v1190a::CheckCommunication(int ntdcs){//Check the communication with the micro controller
    int check = 0;

    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_READ_SPARE_V1190A,"CheckCommunication");
        check = read_op_reg(Address[tdc],"CheckCommunication");
        if(check != 0x5555){
            string tdcnumber = intTostring(tdc);
            MSG_ERROR("[TDC"+tdcnumber+"-ERROR] Communication error");
        }
    }
}

// *************************************************************************************************************

void v1190a::SetTDCTestMode(Data16 mode, int ntdcs){//Enable/Disable TDC test mode
    if(mode == ENABLE)
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc], OPCODE_EN_TEST_MODE_V1190A, "SetTDCTestMode - Step 1");

    else if(mode == DISABLE)
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc], OPCODE_DIS_TEST_MODE_V1190A, "SetTDCTestMode - Step 2");
}

// *************************************************************************************************************

void v1190a::SetTrigMatching(int ntdcs){//Set trigger matching mode
    for(int tdc=0; tdc < ntdcs; tdc++)
        write_op_reg(Address[tdc], OPCODE_TRG_MATCH_V1190A, "SetTrigMatching");
}

// *************************************************************************************************************

void v1190a::SetTrigTimeSubstraction(Data16 mode, int ntdcs){//Enable/Disable substraction of trigger time
    if(mode == ENABLE)
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc], OPCODE_EN_SUB_TRG_V1190A,"SetTrigTimeSubstraction - Step 1");

    else if(mode == DISABLE)
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc], OPCODE_DIS_SUB_TRG_V1190A, "SetTrigTimeSubstraction - Step 2");
}

// *************************************************************************************************************

void v1190a::SetTrigWindowWidth(Uint windowWidth,int ntdcs){
    // Time unit = 25ns
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_WIN_WIDTH_V1190A, "SetTrigWindowWidth - Step 1");
        write_op_reg(Address[tdc], windowWidth, "SetTrigWindowWidth - Step 2");
    }
}

// *************************************************************************************************************

void v1190a::SetTrigWindowOffset(Uint windowOffset, int ntdcs){
    // Time unit = 25ns
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_WIN_OFFSET_V1190A, "SetTrigWindowOffset - Step 1");
        write_op_reg(Address[tdc], windowOffset, "SetTrigWindowOffset - Step 2");
    }
}

// *************************************************************************************************************

void v1190a::SetTrigSearchMargin(Uint searchMargin, int ntdcs){
    // Time unit = 25ns
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_SW_MARGIN_V1190A, "SetTrigSearchMargin - Step 1");
        write_op_reg(Address[tdc], searchMargin, "SetTrigSearchMargin - Step 2");
    }
}

// *************************************************************************************************************

void v1190a::SetTrigRejectionMargin(Uint rejectMargin, int ntdcs){
    // Time unit = 25ns
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_REJ_MARGIN_V1190A, "SetTrigRejectionMargin - Step 1");
        write_op_reg(Address[tdc], rejectMargin, "SetTrigRejectionMargin - Step 2");
    }
}

// *************************************************************************************************************

void v1190a::GetTrigConfiguration(int ntdcs){ //Read and print trigger configuration
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc], OPCODE_READ_TRG_CONF_V1190A, "GetTrigConfiguration - Step 1");

        Data16 MatchWindowWidth,WindowOffset,ExtraSearchWindowWidth,RejectMargin,TriggerTimeSubtraction;
        MatchWindowWidth        = read_op_reg(Address[tdc], "GetTrigConfiguration - Step 2");
        WindowOffset            = read_op_reg(Address[tdc], "GetTrigConfiguration - Step 3");
        ExtraSearchWindowWidth  = read_op_reg(Address[tdc], "GetTrigConfiguration - Step 4");
        RejectMargin            = read_op_reg(Address[tdc], "GetTrigConfiguration - Step 5");
        TriggerTimeSubtraction  = read_op_reg(Address[tdc], "GetTrigConfiguration - Step 6");

        PrintLogV1190(tdc,"Match Window Width",MatchWindowWidth*25);
        PrintLogV1190(tdc,"Window Offset",(65536-WindowOffset)*25);
        PrintLogV1190(tdc,"Extra Search Window Width",ExtraSearchWindowWidth*25);
        PrintLogV1190(tdc,"Reject Margin",RejectMargin*25);
        PrintLogV1190(tdc,"Trigger Time Subtraction",TriggerTimeSubtraction*25);
    }
}

// *************************************************************************************************************

void v1190a::SetTrigConfiguration(IniFile *inifile,int ntdcs){ //Set and print trigger configuration
    // Each parameter is defined taking into account that the time unit is 25ns
    int width = TRIG_DEF_WIDTH_V1990A;
    int offset = TRIG_DEF_OFFSET_V1190A;

    // The width and offset are set according to the beam status. Indeed, in the case the beam is ON,
    // this means the actual run is part of an efficiency scan and thus it needs 600ns (24 clocks)
    // width and -725ns (-29 clocks) offset while in the case of beam OFF, it is likely that the run
    // is part of a Rate scan and needs a longer width (10000ns = 400 clocks) and offset (-10025ns =
    // -401 clocks) to ensure a long integrated time for the rate calculation.

    string runtype = inifile->stringType("General","RunType","rate");
    if(runtype == "rate"){
        width = TRIG_EFF_WIDTH_V1990A;
        offset = TRIG_EFF_OFFSET_V1190A;
    } else if(runtype == "efficiency" || runtype == "noise_reference" || runtype == "test" || runtype == "calibration" || runtype == "impaired"){
        width = TRIG_RATE_WIDTH_V1990A;
        offset = TRIG_RATE_OFFSET_V1190A;
    } else
        MSG_WARNING("[IniFile-WARNING] Wrong run type (rate,efficiency,noise_rate,calibration,impaired or test) : used default TDC settings");

    SetTrigWindowWidth(width,ntdcs);
    SetTrigWindowOffset(offset,ntdcs);
    SetTrigSearchMargin(inifile->intType("TDCSettings","TriggerExtraSearchMargin",TRIG_SRCH_MARGIN_V1190A),ntdcs);
    SetTrigRejectionMargin(inifile->intType("TDCSettings","TriggerRejectMargin",TRIG_REJ_MARGIN_V1190A),ntdcs);

    GetTrigConfiguration(ntdcs);
}

// *************************************************************************************************************

void v1190a::SetTDCDetectionMode(Data16 mode, int ntdcs){
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_DETECTION_V1190A, "SetTDCDetectionMode - Step 1"); //Edge detection selection
        write_op_reg(Address[tdc],mode, "SetTDCDetectionMode - Step 2");
        write_op_reg(Address[tdc],OPCODE_READ_DETECTION_V1190A, "SetTDCDetectionMode - Step 3"); //Edge detection readout

        PrintLogV1190(tdc,"Edge readout",(read_op_reg(Address[tdc], "SetTDCDetectionMode - Step 4") & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCResolution(Data16 lsb, int ntdcs){ //Resolution readout
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_TR_LEAD_LSB_V1190A, "SetTDCResolution - Step 1"); //Set LSB of leading/trailing edge
        write_op_reg(Address[tdc],lsb, "SetTDCResolution - Step 2");
        write_op_reg(Address[tdc],OPCODE_READ_RES_V1190A, "SetTDCResolution - Step 3"); //Resolution readout

        PrintLogV1190(tdc,"Resolution",(read_op_reg(Address[tdc], "SetTDCResolution - Step 4") & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCDeadTime(Data16 time, int ntdcs){
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_DEAD_TIME_V1190A, "SetTDCDeadTime - Step 1"); //Set channel dead time
        write_op_reg(Address[tdc],time, "SetTDCDeadTime - Step 2");
        write_op_reg(Address[tdc],OPCODE_READ_DEAD_TIME_V1190A, "SetTDCDeadTime - Step 3"); //Channel dead time readout

        PrintLogV1190(tdc,"Channel dead time",(read_op_reg(Address[tdc], "SetTDCDeadTime - Step 4") & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCHeadTrailer(Data16 mode, int ntdcs){ //Enable/Disable TDC header and trailer
    if(mode == 1){
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc],OPCODE_EN_HEAD_TRAILER_V1190A, "SetTDCHeadTrailer - Step 1");
    } else if(mode == 0){
        for(int tdc=0; tdc < ntdcs; tdc++)
            write_op_reg(Address[tdc],OPCODE_DIS_HEAD_TRAILER_V1190A, "SetTDCHeadTrailer - Step 2");
    }

    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_READ_HEAD_TRAILER_V1190A, "SetTDCHeadTrailer - Step 3");

        PrintLogV1190(tdc,"TDC header/trailer status (on/off)",(read_op_reg(Address[tdc], "SetTDCHeadTrailer - Step 4") & 0b1));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCEventSize(Data16 size,int ntdcs){ //Maximum number of hits per event readout
    for(int tdc=0; tdc < ntdcs; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_EVENT_SIZE_V1190A, "SetTDCEventSize - Step 1");
        write_op_reg(Address[tdc],size, "SetTDCEventSize - Step 2");
        write_op_reg(Address[tdc],OPCODE_READ_EVENT_SIZE_V1190A, "SetTDCEventSize - Step 3");

        PrintLogV1190(tdc,"Maximum number of hit/event",(read_op_reg(Address[tdc], "SetTDCEventSize - Step 4") & 0b1111));
    }
}

// *************************************************************************************************************

void v1190a::SwitchChannels(IniFile *inifile, int ntdcs){
    char Connectors[5]="ABCD";
    int StatusList[8];
    int firstchannel = 0;
    int lastchannel = 0;

    //Enable - Disable channels according to configuration file

    for(int tdc=0; tdc < ntdcs; tdc++){
        char groupname[10];
        sprintf(groupname,"TDC%i",tdc);

        write_op_reg(Address[tdc],OPCODE_WRITE_EN_PATTERN_V1190A, "SwitchChannels - Step 1");

        string TDCType = inifile->stringType(groupname,"Type","V1190A");
        int nConnectors = 0;

        if(TDCType == "V1190A") nConnectors = 4;
        else if(TDCType == "V1190B") nConnectors = 2;

        for(int c = 0; c < nConnectors; c++){ //Loop over the 32-channels TDC connectors (A,B,C,D or A,B)
            for(int ch = 0; ch < 2; ch++){ //Loop over the 2 16-channels inputs of each TDC connector
                firstchannel = ch*16;
                lastchannel = firstchannel+15;

                char keyname[15];
                sprintf(keyname,"Status%c%02u-%02u",Connectors[c],firstchannel,lastchannel);

                int status = inifile->intType(groupname,keyname,ENABLE);

                if(status == ENABLE)
                    StatusList[2*c+ch] = 0xFFFF;
                else if(status == DISABLE)
                    StatusList[2*c+ch] = 0x0000;

                write_op_reg(Address[tdc],StatusList[2*c+ch], "SwitchChannels - Step 2");
            }
        }

        //Read status of channels

        write_op_reg(Address[tdc],OPCODE_READ_EN_PATTERN_V1190A, "SwitchChannels - Step 3");
        for(int c = 0; c<4; c++){ //Loop over the 32-channels TDC connectors (A,B,C,D)
            for(int ch = 0; ch<2; ch++){ //Loop over the 2 16-channels inputs of each TDC connector
                firstchannel = ch*16;
                lastchannel = firstchannel+15;

                char tmpname[15];
                sprintf(tmpname,"%c%02u-%02u",Connectors[c],firstchannel,lastchannel);
                string name = string(tmpname);
                PrintLogV1190(tdc,"Status - "+name,(read_op_reg(Address[tdc], "SwitchChannels - Step 4") & 0xFFFF));
            }
        }
    }
}

// *************************************************************************************************************
// Set the chosen IRQ
void v1190a::SetIRQ(Data32 level, Data32 count, int ntdcs) {
    // IRQ lines go from 1 to 7, so line 0 disables the IRQ function
    if (level == DISABLE){
        for(int tdc=0; tdc < ntdcs; tdc++)
            CAENVME_WriteCycle(Handle,Address[tdc]+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);
        return;
    }

    // The VME bus has IRQ lines numbered 1 to 7
    if (level < 1 || level > 7) {
        MSG_ERROR("[v1190-ERROR] Tried to enable invalid IRQ");
        return;
    }

    // Setting 0 makes no sense, and the output buffer can hold 32735 words
    if (count < 1 || count > 32735) {
        unsigned int new_count = count < 1 ? 1 : 32735;

        stringstream ss;
        ss << new_count;
        string log_count = UintTostring(new_count);

        MSG_WARNING("[v1190-WARNING] Tried to enable invalid Almost Full Level");
        MSG_INFO("[v1190] Level capped to "+log_count);
        count = new_count;
    }

    for(int tdc=0; tdc < ntdcs; tdc++){
        // Set the IRQ line we want the module to use
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);

        // Set the output buffer word count we want to be notified at
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_ALMOST_FULL_LVL_V1190A,&count,AddressModifier,DataWidth);
    }
}

// *************************************************************************************************************

void v1190a::SetBlockTransferMode(Data16 mode, int ntdcs) {
    Data16 num = (Data16) (mode==ENABLE ? (BLOCK_SIZE / 20) : 0);

    for(int tdc=0; tdc < ntdcs; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_BLT_EVENT_NUM_V1190A,&num,AddressModifier,DataWidth);
}

// *************************************************************************************************************

void v1190a::Set(IniFile * inifile, int ntdcs){
    Reset(ntdcs);
    TestWR(TEST_WR,ntdcs);
    CheckTDCStatus(ntdcs);
    CheckCommunication(ntdcs);

    SetTDCTestMode(inifile->intType("TDCSettings","TdcTestMode",DISABLE),ntdcs);
    SetBlockTransferMode(inifile->intType("TDCSettings","BLTMode",ENABLE),ntdcs);

    SetTrigMatching(ntdcs);
    SetTrigTimeSubstraction(inifile->intType("TDCSettings","TriggerTimeSubstraction",ENABLE),ntdcs);
    SetTrigConfiguration(inifile,ntdcs);

    SetTDCDetectionMode(inifile->intType("TDCSettings","TdcDetectionMode",EdgeMode_Both),ntdcs);
    SetTDCResolution(inifile->intType("TDCSettings","TdcResolution",Res_100ps),ntdcs);
    SetTDCDeadTime(inifile->intType("TDCSettings","TdcDeadTime",DT_5ns),ntdcs);

    SetTDCHeadTrailer(inifile->intType("TDCSettings","TdcHeadTrailer",ENABLE),ntdcs);
    SetTDCEventSize(inifile->intType("TDCSettings","TdcEventSize",HITS_UNLIMITED),ntdcs);

    SwitchChannels(inifile,ntdcs);

    SetIRQ(1,BLOCK_SIZE,ntdcs);
}

// *************************************************************************************************************
// As the name says, it decodes the error code previously returned and saved
// into Status. Returns a string describing the error code. See CAEMVMElib.h .

void v1190a::CheckStatus(CVErrorCodes status) const{
    // These exceptions will get passed up the call chain
    // This provides more flexible error handling, as the return value method is more of a C-ism
    switch (status){
        case cvBusError:
            MSG_ERROR("[v1190] VME bus error");
        case cvCommError:
            MSG_ERROR("[v1190] Communication error");
        case cvGenericError:
            MSG_ERROR("[v1190] General VME library error");
        case cvInvalidParam:
            MSG_ERROR("[v1190] Invalid parameter passed to VME library");
        case cvTimeoutError:
            MSG_ERROR("[v1190] Request timed out");
        default:
            return;
    }
}

// *************************************************************************************************************

int v1190a::ReadBlockD32(Uint tdc, const Data16 address, Data32 *data, const unsigned int words, bool ignore_berr) {
    int read;

    CVErrorCodes ret = CAENVME_BLTReadCycle(Handle, address + Address[tdc], data, words * 4, cvA32_U_BLT, cvD32, &read);
    if (!ignore_berr || ret < cvBusError)
        CheckStatus(ret);

    return (read/4);
}

// *************************************************************************************************************

Uint v1190a::Read(RAWData *DataList, int ntdcs){
    vector<Data16> EventStored;
    EventStored.clear();

    Uint MaxEventStored = 0;

    for(int tdc=0; tdc < ntdcs; tdc++){
        EventStored.push_back(0);

        //Get the number of trigger in TDC memory
        CheckStatus(CAENVME_ReadCycle(Handle, Address[tdc]+ADD_EVENT_STORED_V1190A, &EventStored[tdc], cvA32_U_DATA, cvD16 ));

        Data32 words[BLOCK_SIZE] = {0};
        Uint Count = EventStored[tdc];
        Data32 channel = 9999;
        Data32 timing = 8888;

        int EventCount = -99;
        int nHits = -88;
        vector<int> TDCCh;
        vector<float> TDCTS;

        TDCCh.clear();
        TDCTS.clear();

        //Sometimes, the header is not weel read out in the buffer. To control this, the previous
        //good word having been read out to know when this happens. When this happens, the bool
        //Header stays at false.
//        Data32 previous_word = 0; //used for debug purpose
        bool Header = false;

        while(Count > 0){
            int words_read = ReadBlockD32(tdc,ADD_OUT_BUFFER_V1190A, words, BLOCK_SIZE, true);

            for(int w=0; w<words_read && Count>0 ; w++){
                Data32 word_type = words[w] & STATUS_TDC_V1190A;

                switch(word_type){

                    case GLOBAL_HEADER_V1190A: {
                        //Get the event count from the global header (very first word)
                        EventCount = ((words[w]>>5) & 0x3FFFFF) + 1;

                        //Save GLOBAL_HEADER as the last good word and Header is true
                        //previous_word = word_type;
                        Header = true;

                        break;
                    }
                    case GLOBAL_TRAILER_V1190A: {
                        if(!Header) break;
                        //The global trailer is the very last word of an event. At that
                        //point the number of hits in the event is known.
                        nHits = TDCCh.size();

                        //Put all the data in the RAWData lists

                        //When the event entry is not yet created in the data lists, a new
                        //entry is created. The difference in entries is saved. If it is
                        //greater than 1 (what shouldn't be possible), a log error message
                        //is printed out but the acquisition isn't stopped and an empty
                        //entry is created instead of the missing one.
                        //Else, the data is added to the already existing entry.
                        if(EventCount > (int)DataList->EventList->size()){
                            int Difference = EventCount - (int)DataList->EventList->size();

                            if(Difference > 1)
                                MSG_WARNING("[DAQ-WARNING] Some events are not well written : the trigger rate is too high");
                            for(int i=0; i<Difference-1; i++){
                                DataList->EventList->push_back(EventCount-Difference+i);
                                DataList->NHitsList->push_back(0);
                                DataList->ChannelList->push_back({0});
                                DataList->TimeStampList->push_back({0.});
                            }
                            DataList->EventList->push_back(EventCount);
                            DataList->NHitsList->push_back(nHits);
                            DataList->ChannelList->push_back(TDCCh);
                            DataList->TimeStampList->push_back(TDCTS);
                        } else {
                            Uint it = EventCount-1;
                            DataList->EventList->at(it) = EventCount;
                            DataList->NHitsList->at(it) = DataList->NHitsList->at(it) + nHits;
                            DataList->ChannelList->at(it).insert(DataList->ChannelList->at(it).end(),TDCCh.begin(),TDCCh.end());
                            DataList->TimeStampList->at(it).insert(DataList->TimeStampList->at(it).end(),TDCTS.begin(),TDCTS.end());
                        }

                        //Decrement the counter and if it reaches 0 compare the last event number to
                        //the max event number known
                        Count--;
                        if(Count == 0 && EventCount > (int)MaxEventStored)
                            MaxEventStored = EventCount;

                        //The reinitialise our temporary variables
                        EventCount = -99;
                        nHits = -88;
                        TDCCh.clear();
                        TDCTS.clear();

                        //Save GLOBAL_TRAILER as the last good word
                        //previous_word = word_type;

                        break;
                    }
                    case TDC_DATA_V1190A: {
                        if(!Header) break;
                        //each TDC module separated by 1000 in channel numbers
                        //check which TDC are included in the data taking and
                        //adapt using an offset to always write the data at the
                        //right place
                        int offset = (Address[0] / 0x11110000);
                        channel = ((words[w]>>19) & 0x7F) + (tdc+offset)*1000;
                        TDCCh.push_back(channel);

                        timing = words[w] & 0x7FFFF;
                        TDCTS.push_back((float)timing/10.);

                        //Save TDC_DATA as the last good word
                        //previous_word = word_type;

                        break;
                    }
                    case TDC_HEADER_V1190A:{
                        if(!Header) break;

                        //Save TDC_HEADER as the last good word
                        //previous_word = word_type;
                        break;
                    }
                    case TDC_ERROR_V1190A:{
                        if(!Header) break;

                        //Save TDC_ERROR as the last good word
                        //previous_word = word_type;
                        break;
                    }
                    case TDC_TRAILER_V1190A:{
                        if(!Header) break;

                        //Save TDC_TRAILER as the last good word
                        //previous_word = word_type;
                        break;
                    }
                    case GLOBAL_TRIGGER_TIME_TAG_V1190A:{
                        if(!Header) break;

                        //Save GLOBAL_TRIGGER_TIME as the last good word
                        //previous_word = word_type;
                        break;
                    }
                    default:{
                        break;
                    }
                }
                if(Count == 0) break;
            }
        }
    }
    return MaxEventStored;
}
