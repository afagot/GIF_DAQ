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
#include <vector>
#include <string>
#include <unistd.h>

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"

#include "../include/v1190a.h"
#include "../include/v1718.h"
#include "../include/MsgSvc.h"

using namespace std;

v1190a::v1190a(long handle, IniFile * inifile){
    for(int tdc=0; tdc<MAXNTDC; tdc++){
        char groupname[10];
        sprintf(groupname,"TDC%i",tdc);
        Address[tdc]=inifile->addressType(groupname,"BaseAddress",BASEV1190A[tdc]); //See page 35
    }
    DataWidth=cvD16;              //See in CAENVMEtypes.h CVDataWidth enum
    AddressModifier=cvA24_U_DATA; //See in CAENVMEtypes.h CVAddressModifier enum
    Handle=handle;
}

// *************************************************************************************************************

v1190a::~v1190a(){}

// *************************************************************************************************************

Data16 v1190a::read_op_reg(Data32 address){
    //Checks the Read OK bit
    int time = 0;
    Data16 ro_bit;

    do{
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&ro_bit,AddressModifier,DataWidth);
        time++;
    } while(ro_bit != READ_OK && time < 100000);

    if(time == 100000){
        MSG_ERROR("[v1190a]: Reads - timeout error\n");
        exit(0);
    } else {
        sleep(1);      /*** delay 12ms, internal delay ***/
        //Reads opcode
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_V1190A,&ro_bit,AddressModifier,DataWidth );
        return ro_bit;
    }
}

// *************************************************************************************************************

Data16 v1190a::write_op_reg(Data32 address, int code){
    //Checks the Write OK bit
    int time = 0;
    Data16 wo_bit;

    do{
        CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&wo_bit,AddressModifier,DataWidth );
        time++;
    } while(wo_bit != WRITE_OK && time < 100000);

    if(time == 100000){
        MSG_ERROR("[v1190a]: Write opcode - timeout error\n");
        exit(0);
    } else {
        sleep(1);      /*** delay 12 msec, internal delay ***/
        //Writes opcode = code
        CAENVME_WriteCycle(Handle,address+ADD_MICRO_V1190A,&code,AddressModifier,DataWidth);
        return 0;
    }
}

// *************************************************************************************************************

void v1190a::Reset(){ //Reset the TDCs (Software clear) and wait 1s
    MSG_INFO("[v1190a]: Reset the TDCs...\n");
    int reset=0x0;

    for(int tdc=0; tdc < MAXNTDC; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_MOD_RESET_V1190A,&reset,AddressModifier,DataWidth);
    sleep(1);
}

// *************************************************************************************************************

void v1190a::Clear(){ //Clear the TDC Buffers and wait 1s
    MSG_INFO("[v1190a]: Clear the TDC Buffers...\n");
    int clear=0x0;

    for(int tdc=0; tdc < MAXNTDC; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_SW_CLEAR_V1190A,&clear,AddressModifier,DataWidth);
    sleep(1);
}

// *************************************************************************************************************

void v1190a::TestWR(Data16 value){ //Test : try to write/read 16 bit word
    for(int tdc=0; tdc < MAXNTDC; tdc++){
        Data16 test=value;
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

        test=0;
        CAENVME_ReadCycle(Handle,Address[tdc]+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

        if(test == value) MSG_INFO("[TDC%i]:  Result of W/R test is %X (%X tested)\n",tdc,test,value);
        else MSG_ERROR("[TDC%i]:  Result of W/R test is %X (%X tested)\n",tdc,test,value);
    }
}

// *************************************************************************************************************

void v1190a::CheckStatus(){ //Status of the TDCs (Status register)
    MSG_INFO("[v1190a]: Status of the TDCs...\n");
    Data16 status;

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        CAENVME_ReadCycle(Handle,Address[tdc]+ADD_STATUS_V1190A,&status,AddressModifier,DataWidth);
        MSG_INFO("[TDC%i]:  Status is %X\n",tdc,status);
    }
}

// *************************************************************************************************************

void v1190a::CheckCommunication(){//Check the communication with the micro controller
    MSG_INFO("[v1190a]: Control of the communication with the uController gives:\n");
    int check = 0;

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_READ_SPARE_V1190A);
        check = read_op_reg(Address[tdc]);
        if(check == 0x5555) MSG_INFO("[TDC%i]:\t 0x%04X (OK)\n",tdc,check);
        else MSG_ERROR("[TDC%i]:\t 0x%4X (NOT OK != 0x5555)\n",tdc,check);
    }
}

// *************************************************************************************************************

void v1190a::SetTDCTestMode(Data16 mode){//Enable/Disable TDC test mode
    if(mode == ENABLE)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc], OPCODE_EN_TEST_MODE_V1190A);

    else if(mode == DISABLE)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc], OPCODE_DIS_TEST_MODE_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigMatching(){//Set trigger matching mode
    for(int tdc=0; tdc < MAXNTDC; tdc++)
        write_op_reg(Address[tdc], OPCODE_TRG_MATCH_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigTimeSubstraction(Data16 mode){//Enable/Disable substraction of trigger time
    if(mode == ENABLE)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc], OPCODE_EN_SUB_TRG_V1190A);

    else if(mode == DISABLE)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc], OPCODE_DIS_SUB_TRG_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigWindowWidth(Uint windowWidth){
    // Time unit = 25ns
    Uint windowWidthns = windowWidth*25;
    MSG_INFO("[v1190a]: Window width is set to 0x%02X (%dns)\n",windowWidth,windowWidthns);

    for(Uint tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_WIN_WIDTH_V1190A);
        write_op_reg(Address[tdc], windowWidth);
    }
}

// *************************************************************************************************************

void v1190a::SetTrigWindowOffset(Uint windowOffset){
    // Time unit = 25ns
    Uint windowOffsetns = windowOffset*25;
    MSG_INFO("[v1190a]: Window Offset is set to 0x%08X (%dns)\n",windowOffset,windowOffsetns);

    for(Uint tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_WIN_OFFSET_V1190A);
        write_op_reg(Address[tdc], windowOffset);
    }
}

// *************************************************************************************************************

void v1190a::SetTrigSearchMargin(Uint searchMargin){
    // Time unit = 25ns
    Uint searchMarginns = searchMargin*25;
    MSG_INFO("[v1190a]: Extra search margin is set to 0x%02X (%dns)\n",searchMargin,searchMarginns);

    for(Uint tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_SW_MARGIN_V1190A);
        write_op_reg(Address[tdc], searchMargin);
    }
}

// *************************************************************************************************************

void v1190a::SetTrigRejectionMargin(Uint rejectMargin){
    // Time unit = 25ns
    Uint rejectMarginns = rejectMargin*25;
    MSG_INFO("[v1190a]: Rejection margin is set to 0x%02X (%dns)\n",rejectMargin,rejectMarginns);

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc], OPCODE_SET_REJ_MARGIN_V1190A);
        write_op_reg(Address[tdc], rejectMargin);
    }
}

// *************************************************************************************************************

void v1190a::GetTrigConfiguration(){ //Read and print trigger configuration
    MSG_INFO("[v1190a]: Trigger control configuration:\n");

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc], OPCODE_READ_TRG_CONF_V1190A);

        Data16 MatchWindowWidth,WindowOffset,ExtraSearchWindowWidth,RejectMargin,TriggerTimeSubtraction;
        MatchWindowWidth        =read_op_reg(Address[tdc]);
        WindowOffset            =read_op_reg(Address[tdc]);
        ExtraSearchWindowWidth  =read_op_reg(Address[tdc]);
        RejectMargin            =read_op_reg(Address[tdc]);
        TriggerTimeSubtraction  =read_op_reg(Address[tdc]);

        MSG_INFO("[TDC%i]:\t Match Window Width :        0x%02X\n"  ,tdc,MatchWindowWidth);
        MSG_INFO("[TDC%i]:\t Window Offset :             0x%08X\n"  ,tdc,WindowOffset);
        MSG_INFO("[TDC%i]:\t Extra Search Window Width : 0x%02X\n"  ,tdc,ExtraSearchWindowWidth);
        MSG_INFO("[TDC%i]:\t Reject Margin :             0x%02X\n"  ,tdc,RejectMargin);
        MSG_INFO("[TDC%i]:\t Trigger Time Subtraction :  %d\n"      ,tdc,TriggerTimeSubtraction);
    }
}

// *************************************************************************************************************

void v1190a::SetTrigConfiguration(IniFile *inifile){ //Set and print trigger configuration
    // Each parameter is defined taking into account that the time unit is 25ns
    SetTrigWindowWidth(inifile->intType("TDCSettings","TriggerWindowWidth",TRIG_WIN_WIDTH_V1990A));
    SetTrigWindowOffset(inifile->intType("TDCSettings","TriggerWindowOffset",TRIG_WIN_OFFSET_V1190A));
    SetTrigSearchMargin(inifile->intType("TDCSettings","TriggerExtraSearchMargin",TRIG_SRCH_MARGIN_V1190A));
    SetTrigRejectionMargin(inifile->intType("TDCSettings","TriggerRejectMargin",TRIG_REJ_MARGIN_V1190A));

    GetTrigConfiguration();
}

// *************************************************************************************************************

void v1190a::SetTDCDetectionMode(Data16 mode){
    MSG_INFO("[v1190a]: Edge detection setting\n");

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_DETECTION_V1190A); //Edge detection selection
        write_op_reg(Address[tdc],mode);
        write_op_reg(Address[tdc],OPCODE_READ_DETECTION_V1190A); //Edge detection readout
        MSG_INFO("[TDC%i]:\t Edge readout : %1X\n",tdc,(read_op_reg(Address[tdc]) & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCResolution(Data16 lsb){ //Resolution readout
    MSG_INFO("[v1190a]: Channel resolution setting\n");

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_TR_LEAD_LSB_V1190A); //Set channel dead time
        write_op_reg(Address[tdc],lsb);
        write_op_reg(Address[tdc],OPCODE_READ_RES_V1190A);
        MSG_INFO("[TDC%i]:\t Resoltion : %1X\n",tdc,(read_op_reg(Address[tdc]) & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCDeadTime(Data16 time){
    MSG_INFO("[v1190a]: Channel dead time setting\n");

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_DEAD_TIME_V1190A); //Set channel dead time
        write_op_reg(Address[tdc],time);
        write_op_reg(Address[tdc],OPCODE_READ_DEAD_TIME_V1190A); //Channel dead time readout
        MSG_INFO("[TDC%i]:\t Channel dead time : %1X\n",tdc,(read_op_reg(Address[tdc]) & 0b11));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCHeadTrailer(Data16 mode){ //Enable/Disable TDC header and trailer
    MSG_INFO("[v1190a]: TDC header configuration\n");

    if(mode == 1)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc],OPCODE_EN_HEAD_TRAILER_V1190A);
    else if(mode == 0)
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            write_op_reg(Address[tdc],OPCODE_DIS_HEAD_TRAILER_V1190A);

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_READ_HEAD_TRAILER_V1190A);
        MSG_INFO("[TDC%i]:\t TDC header/trailer status (on/off) : %1X\n",tdc,(read_op_reg(Address[tdc]) & 0b1));
    }
}

// *************************************************************************************************************

void v1190a::SetTDCEventSize(Data16 size){ //Maximum number of hits per event readout
    MSG_INFO("[v1190a]: TDC event size configuration\n");

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        write_op_reg(Address[tdc],OPCODE_SET_EVENT_SIZE_V1190A);
        write_op_reg(Address[tdc],size);
        write_op_reg(Address[tdc],OPCODE_READ_EVENT_SIZE_V1190A);
        MSG_INFO("[TDC%i]:\t Maximum number of hit/event : %1X\n",tdc,(read_op_reg(Address[tdc]) & 0b1111));
    }
}

// *************************************************************************************************************

void v1190a::SwitchChannels(IniFile *inifile){
    MSG_INFO("[v1190a]: TDC Channels enabling\n");

    char Connectors[5]="ABCD";
    int StatusList[8];
    int firstchannel = 0;
    int lastchannel = 0;

    //Enable - Disable channels according to configuration file

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        char groupname[10];
        sprintf(groupname,"TDC%i",tdc);

        write_op_reg(Address[tdc],OPCODE_WRITE_EN_PATTERN_V1190A);
        for(int c = 0; c < 4; c++){ //Loop over the 32-channels TDC connectors (A,B,C,D)
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

                write_op_reg(Address[tdc],StatusList[2*c+ch]);
            }
        }

        //Read status of channels

        write_op_reg(Address[tdc],OPCODE_READ_EN_PATTERN_V1190A);
        for(int c = 0; c<4; c++){ //Loop over the 32-channels TDC connectors (A,B,C,D)
            for(int ch = 0; ch<2; ch++){ //Loop over the 2 16-channels inputs of each TDC connector
                firstchannel = ch*16;
                lastchannel = firstchannel+15;

                char name[15];
                sprintf(name,"%c%02u-%02u",Connectors[c],firstchannel,lastchannel);
                MSG_INFO("[TDC%i-%s]:\t 0x%04X\n",tdc,name,(read_op_reg(Address[tdc]) & 0xFFFF));
            }
        }
    }
}

// *************************************************************************************************************
// Set the chosen IRQ
void v1190a::SetIRQ(Data32 level, Data32 count) {
    // IRQ lines go from 1 to 7, so line 0 disables the IRQ function
    if (level == DISABLE){
        MSG_INFO("[v1190a]: Disabling IRQ levels\n");
        for(int tdc=0; tdc < MAXNTDC; tdc++)
            CAENVME_WriteCycle(Handle,Address[tdc]+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);
        return;
    }

    // The VME bus has IRQ lines numbered 1 to 7
    if (level < 1 || level > 7) {
        MSG_ERROR("[v1190a]: Tried to enable invalid IRQ\n");
        return;
    }

    // Setting 0 makes no sense, and the output buffer can hold 32735 words
    if (count < 1 || count > 32735) {
        unsigned int new_count = count < 1 ? 1 : 32735;
        MSG_WARNING("[v1190a]: Tried to enable invalid Almost Full Level\n");
        MSG_INFO("[v1190a]: Level capped to %u\n",new_count);
        count = new_count;
    }

    MSG_INFO("[v1190a]: Enabling IRQ level %u\n",level);

    for(int tdc=0; tdc < MAXNTDC; tdc++){
        // Set the IRQ line we want the module to use
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);

        // Set the output buffer word count we want to be notified at
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_ALMOST_FULL_LVL_V1190A,&count,AddressModifier,DataWidth);
    }
}

// *************************************************************************************************************

void v1190a::SetBlockTransferMode(Data16 mode) {
    if(mode == ENABLE) MSG_INFO("[v1190a]: Enabling block transfer\n");
    else if(mode == DISABLE) MSG_INFO("[v1190a]: Disabling block transfer\n");

    Data16 num = (Data16) (mode==ENABLE ? (BLOCK_SIZE / 20) : 0);

    for(int tdc=0; tdc < MAXNTDC; tdc++)
        CAENVME_WriteCycle(Handle,Address[tdc]+ADD_BLT_EVENT_NUM_V1190A,&num,AddressModifier,DataWidth);
}

// *************************************************************************************************************

void v1190a::Set(IniFile * inifile,v1718 *vme){
    MSG_INFO("[v1190a]: START TO SET THE TDCs\n");

    Reset();
    TestWR(TEST_WR);
    CheckStatus();
    CheckCommunication();
    SetTDCTestMode(inifile->intType("TDCSettings","TdcTestMode",DISABLE));
    SetBlockTransferMode(inifile->intType("TDCSettings","BLTMode",ENABLE));

    SetTrigMatching();
    SetTrigTimeSubstraction(inifile->intType("TDCSettings","TriggerTimeSubstraction",ENABLE));
    SetTrigConfiguration(inifile);

    SetTDCDetectionMode(inifile->intType("TDCSettings","TdcDetectionMode",EdgeMode_Both));
    SetTDCResolution(inifile->intType("TDCSettings","TdcResolution",Res_100ps));
    SetTDCDeadTime(inifile->intType("TDCSettings","TdcDeadTime",DT_5ns));

    SetTDCHeadTrailer(inifile->intType("TDCSettings","TdcHeadTrailer",ENABLE));
    SetTDCEventSize(inifile->intType("TDCSettings","TdcEventSize",HITS_UNLIMITED));

    SwitchChannels(inifile);

    SetIRQ(1,BLOCK_SIZE);

    //Turn ON the VME interface output pulser A on output 0 while clearing
    //the TDC buffers. The signal is used as VETO for the trigger signal
    //and thus there is no data acquired during the clearing time and the
    //TDCs can start being synchronized.
    vme->SendBUSY(ON);
    Clear();
    vme->SendBUSY(OFF);
}

// *************************************************************************************************************
// As the name says, it decodes the error code previously returned and saved
// into Status. Returns a string describing the error code. See CAEMVMElib.h .

void v1190a::CheckStatus(CVErrorCodes status) const{
    // These exceptions will get passed up the call chain
    // This provides more flexible error handling, as the return value method is more of a C-ism
    switch (status){
        case cvBusError:
            MSG_ERROR("[v1718]: \t VME bus error\n");
        case cvCommError:
            MSG_ERROR("[v1718]: \t Communication error\n");
        case cvGenericError:
            MSG_ERROR("[v1718]: \t General VME library error\n");
        case cvInvalidParam:
            MSG_ERROR("[v1718]: \t Invalid parameter passed to VME library\n");
        case cvTimeoutError:
            MSG_ERROR("[v1718]: \t Request timed out\n");
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

Uint v1190a::Read(RAWData *DataList){
    Data16 EventStored[MAXNTDC] = {0};
    Uint MaxEventStored = 0;

    for(Uint tdc=0; tdc < MAXNTDC; tdc++){
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
        Data32 previous_word = 0;
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
                        previous_word = word_type;
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
                                MSG_ERROR("[TDC%i] : \t %i events are missing. Event %d is actual event.",tdc,Difference-1,EventCount);
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
                        previous_word = word_type;

                        break;
                    }
                    case TDC_DATA_V1190A: {
                        if(!Header) break;
                        //each TDC module separated by 1000 in channel numbers
                        channel = ((words[w]>>19) & 0x7F) + tdc*1000;
                        TDCCh.push_back(channel);

                        timing = words[w] & 0x7FFFF;
                        TDCTS.push_back((float)timing/10.);

                        //Save TDC_DATA as the last good word
                        previous_word = word_type;

                        break;
                    }
                    case TDC_HEADER_V1190A:{
                        if(!Header) break;

                        //Save TDC_HEADER as the last good word
                        previous_word = word_type;
                        break;
                    }
                    case TDC_ERROR_V1190A:{
                        if(!Header) break;

                        //Save TDC_ERROR as the last good word
                        previous_word = word_type;
                        break;
                    }
                    case TDC_TRAILER_V1190A:{
                        if(!Header) break;

                        //Save TDC_TRAILER as the last good word
                        previous_word = word_type;
                        break;
                    }
                    case GLOBAL_TRIGGER_TIME_TAG_V1190A:{
                        if(!Header) break;

                        //Save GLOBAL_TRIGGER_TIME as the last good word
                        previous_word = word_type;
                        break;
                    }
                    default:{
                        MSG_ERROR("[TDC%i] : \t Encountered unknown word type while processing events - %08X - Previous word - %08X\n",tdc,word_type,previous_word);
                        break;
                    }
                }
                if(Count == 0) break;
            }
        }
    }
    return MaxEventStored;
}
