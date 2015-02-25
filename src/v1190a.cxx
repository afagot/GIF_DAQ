// *************************************************************************************************************
// *   v1190a 
// *   Alexis Fagot
// *   20/01/2015
// *   Based on v1290a file from :
// *   Avia Raviv & Y. Benhammou
// *   14/03/2012
// *************************************************************************************************************

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
    CAENVME_WriteCycle(Handle,Address+ADD_SW_CLEAR_V1190A,&reset,AddressModifier,DataWidth);
    sleep(2);
}

// *************************************************************************************************************

void v1190a::TestWR(Data16 value){ //Test : try to write/read 16 bit word
    Data16 test=value;
    CAENVME_WriteCycle(Handle,Address+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

    test=0;
    CAENVME_ReadCycle(Handle,Address+ADD_DUMMY16_V1190A,&test,AddressModifier,DataWidth);

    printf("The result of W/R test is : %d (%d tested)\n\n",test,value);
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
    if(mode == 1)
        write_op_reg(Address, OPCODE_EN_TEST_MODE_V1190A);
    else if(mode == 0)
        write_op_reg(Address, OPCODE_DIS_TEST_MODE_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigMatching(){//Set trigger matching mode
    write_op_reg(Address, OPCODE_TRG_MATCH_V1190A);
}

// *************************************************************************************************************

void v1190a::SetTrigTimeSubstraction(Data16 mode){//Enable/Disable substraction of trigger time
    if(mode == 1)
        write_op_reg(Address, OPCODE_EN_SUB_TRG_V1190A);
    else if(mode == 0)
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
    this->SetTrigWindowWidth(inifile->intType("TDC","TriggerWindowWidth",TRIG_WIN_WIDTH_V1990A));
    this->SetTrigWindowOffset(inifile->intType("TDC","TriggerWindowOffset",TRIG_WIN_OFFSET_V1190A));
    this->SetTrigSearchMargin(inifile->intType("TDC","TriggerExtraSearchMargin",TRIG_SRCH_MARGIN_V1190A));
    this->SetTrigRejectionMargin(inifile->intType("TDC","TriggerRejectMargin",TRIG_REJ_MARGIN_V1190A));

    this->GetTrigConfiguration();
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
    cout << "Edge detection set to trailing and leading (0b11)\n";
    write_op_reg(Address,OPCODE_SET_DETECTION_V1190A);              //Edge detection selection
    write_op_reg(Address,mode);
    write_op_reg(Address,OPCODE_READ_DETECTION_V1190A);             //Edge detection readout
    printf("Edge readout :                       %1X\n",(read_op_reg(Address) & 0b11));
}

// *************************************************************************************************************

void v1190a::SetTDCResolution(Data16 lsb){ //Resolution readout
    cout << "Channel resolution set to 100ps (10)\n";
    write_op_reg(Address,OPCODE_SET_TR_LEAD_LSB_V1190A);              //Set channel dead time
    write_op_reg(Address,lsb);
    write_op_reg(Address,OPCODE_READ_RES_V1190A);
    printf("Resoltion :                          %1X\n\n",(read_op_reg(Address) & 0b11));
}

// *************************************************************************************************************

void v1190a::SetTDCDeadTime(Data16 time){
    cout << "Channel dead time between hits set to 5ns (00)\n";
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
            printf("Channels enabled (%s):            0x%04X\n",name,(read_op_reg(Address) & 0xFFFF));
        }
    }
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
}

// *************************************************************************************************************

bool v1190a::IsSetStatusReg(Data32 aBit,v1718 * vme){
    Data16 Data;
    Data = vme->ReadShort(Address + ADD_STATUS_V1190A);
    if(vme->GetStatus() != cvSuccess )
        cerr << "\n[ERR:ModuleV1190A]: Cannot read status register.\n"<< vme->GetError() << endl;

    return ( Data & ( 1 << aBit ) ) != 0;
}

// *************************************************************************************************************

Uint v1190a::Read(v1718 * vme){

    //printf("Status register :     %X\n",IsSetStatusReg(0b11,vme));

    Data16 EventStored;
    CAENVME_ReadCycle(Handle, Address+ADD_EVENT_STORED_V1190A, &EventStored, cvA32_U_DATA, cvD16 );
    if(EventStored > 0)
        printf("N Event stored :      %d\n", EventStored);

    Data32 data;
    Uint Spills = 0;

    bool IsPrinted(false);

    while( EventStored != 0)
    {
        CAENVME_ReadCycle(Handle,Address+ADD_OUT_BUFFER_V1190A,&data,cvA32_U_DATA,cvD32);

        switch(data & STATUS_TDC_V1190A)
        {
            case(GLOBAL_HEADER_V1190A):
                cout << "GLOBAL HEADER " ;
                cout << " Event Count : " << ((data>>5) & 0x3FFFFF)<<endl;
                Spills++;
                break;
            case(GLOBAL_TRAILER_V1190A):
                cout <<"GLOBAL TRAILER ";
                cout <<"Word Count " << (data & 0xFFF)<<endl;
                break;
            case(TDC_HEADER_V1190A):
                if(!IsPrinted){
                    cout <<"TDC HEADER ";
                    cout <<"Event id : " << ( (data>>12) & 0xFFF)<<endl;
                    IsPrinted = true;
                }
                break;
            case(TDC_DATA_V1190A):
                cout <<"Data ";
                cout << " Rise/Fall : "<< ((data>>26) & 0x1) ;
                cout << " Channel : "<<((data>>19) & 0x7F ) ;
                cout << " Value : "<< ( data & 0x7FFFF)<<endl ;
                break;

        }

        CAENVME_ReadCycle(Handle, Address+ADD_EVENT_STORED_V1190A, &EventStored, cvA32_U_DATA, cvD16 );
    }
    return Spills;
}
