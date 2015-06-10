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
#include "../include/MsgSvc.h"

using namespace std;

v1190a::v1190a(long handle, IniFile * inifile)
{
   Address=inifile->addressType("TDC","BaseAddress",BASEV1190A); //See page 35
   DataWidth=cvD16;              //See in CAENVMEtypes.h CVDataWidth enum
   AddressModifier=cvA24_U_DATA; //See in CAENVMEtypes.h CVAddressModifier enum
   Handle=handle;
}

// *************************************************************************************************************

v1190a::~v1190a()
{

}

// *************************************************************************************************************

Data16 v1190a::read_op_reg(Data32 address)
{
   //Checks the Read OK bit
   int time = 0;
   Data16 ro_bit;

   do
   {
       CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&ro_bit,AddressModifier,DataWidth);
       time++;
   } while(ro_bit != READ_OK && time < 100000);

   if(time == 100000)
   {
       printf("Reads - timeout error\n");
       exit(0);
    }
   else
   {
       sleep(1);      /*** delay 12ms, internal delay ***/
       //Reads opcode
       CAENVME_ReadCycle(Handle,address+ADD_MICRO_V1190A,&ro_bit,AddressModifier,DataWidth );
       return ro_bit;
   }
}

// *************************************************************************************************************

Data16 v1190a::write_op_reg(Data32 address, int code)
{
   //Checks the Write OK bit
   int time = 0;
   Data16 wo_bit;
   do
   {
      CAENVME_ReadCycle(Handle,address+ADD_MICRO_HND_V1190A,&wo_bit,AddressModifier,DataWidth );
      time++;
   } while(wo_bit != WRITE_OK && time < 100000);

   if(time == 100000)
   {
      printf("Write opcode - timeout error\n");
      exit(0);
   }
   else
   {
      sleep(1);      /*** delay 12 msec, internal delay ***/
      //Writes opcode = code
      CAENVME_WriteCycle(Handle,address+ADD_MICRO_V1190A,&code,AddressModifier,DataWidth);
      return 0;
    }
}

// *************************************************************************************************************

void v1190a::Reset(){ //Reset the TDC (Software clear) and wait 2 s
    cout << "Reset the TDC...\n\n";
    int reset=0x0;
    CAENVME_WriteCycle(Handle,Address+ADD_MOD_RESET_V1190A,&reset,AddressModifier,DataWidth);
    sleep(2);
}

// *************************************************************************************************************

void v1190a::Clear(){ //Clear the TDC Buffer and wait 1 second
    cout << "Clear the Buffer...\n\n";
    int reset=0x0;
    CAENVME_WriteCycle(Handle,Address+ADD_SW_CLEAR_V1190A,&reset,AddressModifier,DataWidth);
    sleep(1);
}

// *************************************************************************************************************

void v1190a::TestWR(Data16 value){ //Test : try to write/read 16 bit word
    Data16 test=value;
    CAENVME_WriteCycle(Handle,Address+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

    test=0;
    CAENVME_ReadCycle(Handle,Address+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

    printf("The result of W/R test is : %X (%X tested)\n\n",test,value);
}

// *************************************************************************************************************

void v1190a::CheckStatus(){ //Status of the TDC (Status register)
    cout << "Status of the TDC...\n\n";
    Data16 status;
    CAENVME_ReadCycle(Handle,Address+ADD_STATUS_V1190A,&status,AddressModifier,DataWidth);
    printf("TDC STATUS is : %X\n\n",status);
}

// *************************************************************************************************************

void v1190a::CheckCommunication(){//Check the communication with the micro controller
    cout << "Control of the communication with the uController...\n";
    write_op_reg(Address,OPCODE_READ_SPARE_V1190A);
    printf("...gives : 0x%4X  (0x5555 : OK)\n\n", read_op_reg(Address));
}

// *************************************************************************************************************

void v1190a::SetTDCTestMode(Data16 mode){//Enable/Disable TDC test mode
    if(mode == ENABLE)
        write_op_reg(Address, OPCODE_EN_TEST_MODE_V1190A);
    else if(mode == DISABLE)
        write_op_reg(Address, OPCODE_DIS_TEST_MODE_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigMatching(){//Set trigger matching mode
    write_op_reg(Address, OPCODE_TRG_MATCH_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigTimeSubstraction(Data16 mode){//Enable/Disable substraction of trigger time
    if(mode == ENABLE)
        write_op_reg(Address, OPCODE_EN_SUB_TRG_V1190A);
    else if(mode == DISABLE)
        write_op_reg(Address, OPCODE_DIS_SUB_TRG_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigWindowWidth(long windowWidth){
    // Time unit = 25ns
    long windowWidthns = windowWidth*25;
    printf("Window width is set to 0x%02X          (%dns)\n",windowWidth,windowWidthns);
    write_op_reg(Address, OPCODE_SET_WIN_WIDTH_V1190A);
    write_op_reg(Address, windowWidth);
}

// *************************************************************************************************************

void v1190a::SetTrigWindowOffset(long windowOffset){
    // Time unit = 25ns
    long windowOffsetns = windowOffset*25;
    printf("Window Offset is set to 0x%8X   (%dns)\n",windowOffset,windowOffsetns);
    write_op_reg(Address, OPCODE_SET_WIN_OFFSET_V1190A);
    write_op_reg(Address, windowOffset);
}

// *************************************************************************************************************

void v1190a::SetTrigSearchMargin(long searchMargin){
    // Time unit = 25ns
    long searchMarginns = searchMargin*25;
    printf("Extra search margin is set to 0x%02X   (%dns)\n",searchMargin,searchMarginns);
    write_op_reg(Address, OPCODE_SET_SW_MARGIN_V1190A);
    write_op_reg(Address, searchMargin);
}

// *************************************************************************************************************

void v1190a::SetTrigRejectionMargin(long rejectMargin){
    // Time unit = 25ns
    long rejectMarginns = rejectMargin*25;
    printf("Rejection margin is set to 0x%02X      (%dns)\n",rejectMargin,rejectMarginns);
    write_op_reg(Address, OPCODE_SET_REJ_MARGIN_V1190A);
    write_op_reg(Address, rejectMargin);
}

// *************************************************************************************************************

void v1190a::SetTrigConfiguration(IniFile *inifile){ //Set and print trigger configuration
    // Each parameter is defined taking into account that the time unit is 25ns
    SetTrigWindowWidth(inifile->intType("TDC","TriggerWindowWidth",TRIG_WIN_WIDTH_V1990A));
    SetTrigWindowOffset(inifile->intType("TDC","TriggerWindowOffset",TRIG_WIN_OFFSET_V1190A));
    SetTrigSearchMargin(inifile->intType("TDC","TriggerExtraSearchMargin",TRIG_SRCH_MARGIN_V1190A));
    SetTrigRejectionMargin(inifile->intType("TDC","TriggerRejectMargin",TRIG_REJ_MARGIN_V1190A));

    GetTrigConfiguration();
}

// *************************************************************************************************************

void v1190a::GetTrigConfiguration(){ //Read and print trigger configuration
    write_op_reg(Address, OPCODE_READ_TRG_CONF_V1190A);

    Data16 MatchWindowWidth,WindowOffset,ExtraSearchWindowWidth,RejectMargin,TriggerTimeSubtraction;
    MatchWindowWidth        =read_op_reg(Address);
    WindowOffset            =read_op_reg(Address);
    ExtraSearchWindowWidth  =read_op_reg(Address);
    RejectMargin            =read_op_reg(Address);
    TriggerTimeSubtraction  =read_op_reg(Address);

    printf("Match Window Width :                 0x%02X\n"  ,MatchWindowWidth);
    printf("Window Offset :                      0x%8X\n"   ,WindowOffset);
    printf("Extra Search Window Width :          0x%02X\n"  ,ExtraSearchWindowWidth);
    printf("Reject Margin :                      0x%02X\n"  ,RejectMargin);
    printf("Trigger Time Subtraction :           %d\n\n"    ,TriggerTimeSubtraction);
}

// *************************************************************************************************************

void v1190a::SetTDCDetectionMode(Data16 mode){
    cout << "Set edge detection\n";
    write_op_reg(Address,OPCODE_SET_DETECTION_V1190A);              //Edge detection selection
    write_op_reg(Address,mode);
    write_op_reg(Address,OPCODE_READ_DETECTION_V1190A);             //Edge detection readout
    printf("Edge readout :                       %1X\n\n",(read_op_reg(Address) & 0b11));
}

// *************************************************************************************************************

void v1190a::SetTDCResolution(Data16 lsb){ //Resolution readout
    cout << "Set channel resolution\n";
    write_op_reg(Address,OPCODE_SET_TR_LEAD_LSB_V1190A);              //Set channel dead time
    write_op_reg(Address,lsb);
    write_op_reg(Address,OPCODE_READ_RES_V1190A);
    printf("Resoltion :                          %1X\n\n",(read_op_reg(Address) & 0b11));
}

// *************************************************************************************************************

void v1190a::SetTDCDeadTime(Data16 time){
    cout << "Set channel dead time between hits\n";
    write_op_reg(Address,OPCODE_SET_DEAD_TIME_V1190A);              //Set channel dead time
    write_op_reg(Address,time);
    write_op_reg(Address,OPCODE_READ_DEAD_TIME_V1190A);             //Channel dead time readout
    printf("Channel dead time :                  %1X\n\n",(read_op_reg(Address) & 0b11));
}

// *************************************************************************************************************

void v1190a::SetTDCHeadTrailer(Data16 mode){ //Enable/Disable TDC header and trailer
    if(mode == 1)
        write_op_reg(Address,OPCODE_EN_HEAD_TRAILER_V1190A);
    else if(mode == 0)
        write_op_reg(Address,OPCODE_DIS_HEAD_TRAILER_V1190A);

    write_op_reg(Address,OPCODE_READ_HEAD_TRAILER_V1190A);
    printf("TDC header/trailer status (on/off) : %1X\n", (read_op_reg(Address) & 0b1));
}

// *************************************************************************************************************

void v1190a::SetTDCEventSize(Data16 size){ //Maximum number of hits per event readout
    write_op_reg(Address,OPCODE_SET_EVENT_SIZE_V1190A);
    write_op_reg(Address,size);
    write_op_reg(Address,OPCODE_READ_EVENT_SIZE_V1190A);
    printf("Maximum number of hit/event :        %1X\n\n",(read_op_reg(Address) & 0b1111));
}

// *************************************************************************************************************

void v1190a::SwitchChannels(IniFile *inifile){
    char Connectors[5]="ABCD";
    int StatusList[8];
    int firstchannel = 0;
    int lastchannel = 0;

    //Enable - Disable channels according to configuration file

    write_op_reg(Address,OPCODE_WRITE_EN_PATTERN_V1190A);
    for(int c = 0; c < 4; c++){
        for(int ch = 0; ch < 2; ch++){
            firstchannel = ch*16;
            lastchannel = firstchannel+15;

            char keyname[15];
            sprintf(keyname,"Status%c%02u-%02u",Connectors[c],firstchannel,lastchannel);

            int status = inifile->intType("TDC",keyname,ENABLE);

            if(status == ENABLE)
                StatusList[2*c+ch] = 0xFFFF;
            else if(status == DISABLE)
                StatusList[2*c+ch] = 0x0000;

            write_op_reg(Address,StatusList[2*c+ch]);
        }
    }

    //Read status of channels

    write_op_reg(Address,OPCODE_READ_EN_PATTERN_V1190A);
    for(int c = 0; c<4; c++){
        for(int ch = 0; ch<2; ch++){
            firstchannel = ch*16;
            lastchannel = firstchannel+15;

            char name[15];
            sprintf(name,"%c%02u-%02u",Connectors[c],firstchannel,lastchannel);
            printf("Channels enabled (%s):           0x%04X\n",name,(read_op_reg(Address) & 0xFFFF));
        }
    }
}

// *************************************************************************************************************
// Set the chosen IRQ
void v1190a::SetIRQ(Data32 level, Data32 count) {
    // IRQ lines go from 1 to 7, so line 0 disables the IRQ function
    if (level == DISABLE){
        CAENVME_WriteCycle(Handle,Address+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);
        return;
    }

    // The VME bus has IRQ lines numbered 1 to 7
    if (level < 1 || level > 7) {
        MSG_ERROR("Tried to enable invalid IRQ\n");
        return;
    }

    // Setting 0 makes no sense, and the output buffer can hold 32735 words
    if (count < 1 || count > 32735) {
        unsigned int new_count = count < 1 ? 1 : 32735;
        MSG_WARNING("Tried to enable invalid Almost Full Level\n");
        MSG_INFO("Level capped to : ");
        cout << new_count << endl;
        count = new_count;
    }

    // Set the IRQ line we want the module to use
    CAENVME_WriteCycle(Handle,Address+ADD_INT_LEVEL_V1190A,&level,AddressModifier,DataWidth);

    // Set the output buffer word count we want to be notified at
    CAENVME_WriteCycle(Handle,Address+ADD_ALMOST_FULL_LVL_V1190A,&count,AddressModifier,DataWidth);
}

// *************************************************************************************************************

void v1190a::SetBlockTransferMode(Data16 mode) {
    Data16 num = (Data16) (mode==ENABLE ? (BLOCK_SIZE / 20) : 0);
    CAENVME_WriteCycle(Handle,Address+ADD_BLT_EVENT_NUM_V1190A,&num,AddressModifier,DataWidth);
}

// *************************************************************************************************************

void v1190a::Set(IniFile * inifile)
{
    cout << "*************   START TO SET THE TDC   *************\n\n";

    Reset();
    TestWR(TEST_WR);
    CheckStatus();
    CheckCommunication();
    SetTDCTestMode(inifile->intType("TDC","TdcTestMode",DISABLE));
    SetBlockTransferMode(inifile->intType("TDC","BLTMode",ENABLE));

    cout << "************   Trigger configuration   *************\n\n";

    SetTrigMatching();
    SetTrigTimeSubstraction(inifile->intType("TDC","TriggerTimeSubstraction",ENABLE));
    SetTrigConfiguration(inifile);

    cout << "********   TDC edge detection/resolution   *********\n\n";

    SetTDCDetectionMode(inifile->intType("TDC","TdcDetectionMode",EdgeMode_Both));
    SetTDCResolution(inifile->intType("TDC","TdcResolution",Res_100ps));
    SetTDCDeadTime(inifile->intType("TDC","TdcDeadTime",DT_5ns));

    cout << "*****************   TDC readout   ******************\n\n";

    SetTDCHeadTrailer(inifile->intType("TDC","TdcHeadTrailer",ENABLE));
    SetTDCEventSize(inifile->intType("TDC","TdcEventSize",HITS_UNLIMITED));

    cout << "************   Enabling the channels   *************\n\n";

    SwitchChannels(inifile);

    cout << "\n**************   Enable IRQ level 1   **************\n\n";

    SetIRQ(1,10000);

    cout << "\n*********   Clear buffer before starting   *********\n\n";

    Clear();
}

// *************************************************************************************************************

int v1190a::ReadBlockD32(const Data16 address, Data32 *data, const int words, bool ignore_berr) {
    int read;

    CVErrorCodes ret = CAENVME_BLTReadCycle(Handle, address + Address, data, words * 4, cvA32_U_BLT, cvD32, &read);
    if (!ignore_berr || ret < cvBusError)
        cerr << "Error code : " << ret << endl;

    return read / 4;
}

// *************************************************************************************************************

//Uint v1190a::Read(string outputfilename){
Uint v1190a::Read(TTree *RAWDataTree, int EventCount,int nHits,vector<int> *TDCCh,vector<float> *TDCTS){
    Data16 EventStored = 0;
    CAENVME_ReadCycle(Handle, Address+ADD_EVENT_STORED_V1190A, &EventStored, cvA32_U_DATA, cvD16 );

    if(EventStored > 0)
        MSG_INFO("N Event stored : \t%d\n", EventStored);

    Data32 words[BLOCK_SIZE] = {0};
    Uint Spills = 0;
    Uint Count = EventStored;
    Data32 channel, timing;

    //ofstream outputFile(outputfilename.c_str(),ios::app|ios::ate);
    //vector< pair<int,int> > Hits;
    //Hits.clear();
    //Data32 EventCount = -99;

    EventCount = -99;
    nHits = -88;
    TDCCh.clear();
    TDCTS.clear();

    //bool End = false;

    if(outputFile.is_open()){
        while( Count > 0){
            int words_read = ReadBlockD32(ADD_OUT_BUFFER_V1190A, words, BLOCK_SIZE, true);
            for(int w=0; w<words_read; w++){
                switch(words[w] & STATUS_TDC_V1190A){

                case GLOBAL_HEADER_V1190A: {
                    TDCCh.clear();
                    TDCTS.clear();
                    EventCount = ((words[w]>>5) & 0x3FFFFF) + 1;
                    Spills++;

                    break;
                }
                case GLOBAL_TRAILER_V1190A: {
                    Count--;
                    //End = true;
                    if(TDCCh.size() == TDCTS.size()) nHits = TDCCh.size();
                    RAWDataTree->Fill();

                    EventCount = -99;
                    nHit = -88;

                    if(Count==0) return Spills; //This is just temporary cause normally this should be
                                                //already happening thanks to the while loop but not working
                    break;
                }
                case TDC_DATA_V1190A: {
                    channel = (words[w]>>19) & 0x7F;
                    TDCCh.push_back(channel);

                    timing = words[w] & 0x7FFFF;
                    TDCTS.push_back(timing);

                    //Hits.push_back(make_pair(channel,timing));
                    break;
                }
                case TDC_HEADER_V1190A:{
                    break;
                }
                case TDC_ERROR_V1190A:{
                    break;
                }
                case TDC_TRAILER_V1190A:{
                    break;
                }
                case GLOBAL_TRIGGER_TIME_TAG_V1190A:{
                    break;
                }
                default:{
                    //MSG_ERROR("%d : Encountered unknown word type while processing events\n",words[w]);
                    break;
                }

                }
                /*
                if(End){
                    End = false;
                    outputFile << EventCount << '\t' << Hits.size() << '\n';
                    for(int i=0; i<Hits.size(); i++)
                        outputFile << Hits[i].first << '\t' << (float)Hits[i].second/10. << '\n';
                }
                */
            }
        }
    }
    return Spills;
}
