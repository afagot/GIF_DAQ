// *************************************************************************************************************
// *   v1718
// *   Alexis Fagot
// *   20/01/2015
// *   Based on v1718 file from :
// *   Y. Benhammou
// *   27/07/09
// *
// *   Documentation about this VME TDC module can be found at the followin url
// *   http://www.caen.it/servlet/checkCaenManualFile?Id=5385
// *
// *   Comments will often refer to this user manual
// *************************************************************************************************************

#ifndef __LINUX
#define __LINUX
#endif

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

#include "../include/v1718.h"
#include "../include/MsgSvc.h"

using namespace std;

v1718::v1718(IniFile *inifile)
{
   MSG_INFO("Initialization VME...");

   Data32 baseaddress = inifile->addressType("VMEInterface","BaseAddress",BASEV1718) ;

   if( CAENVME_Init(cvV1718, baseaddress, 0, &Handle) != cvSuccess )
   {
      perror("\n\nError opening the v1718 controller device\n");
      exit(1);
   }

   CAENVME_SetFIFOMode(Handle,1);
   this->SetLevel(cvIRQ1);
   this->SetAM(cvA24_U_DATA);
   this->SetDatasize(cvD16);
   this->SetBaseAddress(baseaddress);
   this->SetBlockSize(256);
   this->SetNbCycles(1);
   this->SetAddress(baseaddress);
   MSG_INFO(" OK\n\n");
}

// *************************************************************************************************************

v1718::~v1718()
{
  CAENVME_End(Handle);
}

// *************************************************************************************************************

Data8 v1718::ReadChar( Data32 address ){
    Data8 Data;
    Status = CAENVME_ReadCycle(Handle, address, &Data, cvA32_U_DATA, cvD8 );
    return Data;
}

// *************************************************************************************************************

Data16 v1718::ReadShort( Data32 address ){
    Data16 Data;
    Status = CAENVME_ReadCycle( Handle, address, &Data, cvA32_U_DATA, cvD16 );
    return Data;
}

// *************************************************************************************************************

Data32 v1718::ReadLong( Data32 address ){
    Data32 Data;
    Status = CAENVME_ReadCycle( Handle, address, &Data, cvA32_U_DATA, cvD32 );
    return Data;
}

// *************************************************************************************************************

void v1718::WriteChar( Data32 address, Data8 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD8 );
}

// *************************************************************************************************************

void v1718::WriteShort( Data32 address, Data16 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD16 );
}

// *************************************************************************************************************

void v1718::WriteLong( Data32 address, Data32 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD32 );
}

// *************************************************************************************************************

CVErrorCodes v1718::GetStatus(void)
{
  return Status;
}

// *************************************************************************************************************

string v1718::GetError(void){
    return CAENVME_DecodeError(Status);
}

// *************************************************************************************************************

int v1718::SetAddress(Data16 address)
{
  Address=address;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetAddress(void)
{
  return Address;
}

// *************************************************************************************************************

int v1718::SetData(Data16 data)
{
  Data=data;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetData(void)
{
  return Data;
}

// *************************************************************************************************************

int v1718::SetAM(CVAddressModifier am)
{
  AM=am;
  return 0;
}

// *************************************************************************************************************

CVAddressModifier v1718::GetAM(void)
{
  return AM;
}

// *************************************************************************************************************

int v1718::SetDatasize(CVDataWidth datasize)
{
  DataSize=datasize;
  return 0;
}

// *************************************************************************************************************

CVDataWidth v1718::GetDataSize(void)
{
  return DataSize;
}

// *************************************************************************************************************

int v1718::SetBaseAddress(Data16 baseaddress)
{
  BaseAddress=baseaddress;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetBaseAddress(void)
{
  return BaseAddress;
}

// *************************************************************************************************************

int v1718::SetBlockSize(Data16 blocksize)
{
  BlockSize=blocksize;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetBlockSize(void)
{
  return BlockSize;
}

// *************************************************************************************************************

int v1718::SetNbCycles(Data16 nbcycles)
{
  NbCycles=nbcycles;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetNbCycles(void)
{
  return NbCycles;
}

// *************************************************************************************************************

int v1718::SetLevel(CVIRQLevels level)
{
  Level=level;
  return 0;
}

// *************************************************************************************************************

CVIRQLevels v1718::GetLevel(void)
{
  return Level;
}

// *************************************************************************************************************

int v1718::SetDevice(Data16 device)
{
  Device=device;
  return 0;
}

// *************************************************************************************************************

Data16 v1718::GetDevice(void)
{
  return Device;
}

// *************************************************************************************************************

long v1718::GetHandle(void)
{
  return Handle;
}

// *************************************************************************************************************

int v1718::WriteToVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize)
{
   CVErrorCodes ret;

   if(dtsize == cvD64)
   {
      perror("Can't execute a D64 Write Cycle");
      return -1;
   }
   ret = CAENVME_WriteCycle(Handle,Address,&Data,AM,DataSize);

   switch (ret)
   {
      case cvSuccess       : printf("Cycle(s) completed normally\n");
         return 0;
      case cvBusError      : perror("Bus Error !!!");
         return -1;
      case cvCommError     : perror("Communication Error !!!");
         return -1;
      default              : perror("Unknown Error !!!");
         return -1;
   }
   return 0;
}

// *************************************************************************************************************

int v1718::ReadFromVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize)
{
   CVErrorCodes ret;

   Address=address;
   Data=data;
   AM=am;
   DataSize=dtsize;

   if(dtsize == cvD64)
   {
      perror("Can't execute a D64 Read Cycle");
      return -1;
   }
   ret = CAENVME_ReadCycle(Handle,Address,&Data,AM,DataSize);

   switch (ret)
   {
      case cvSuccess       : printf("Cycle(s) completed normally\n");
         if ( dtsize == cvD32 )  printf("Data Read : %d\n",data);
         if ( dtsize == cvD16 )  printf("Data Read : %d\n",data & 0xffff);
         if ( dtsize == cvD8 )   printf("Data Read : %d\n",data & 0xff);
         break ;
      case cvBusError	   : perror("Bus Error !!!\n");
         break ;
      case cvCommError     : perror("Communication Error !!!\n");
         break ;
      default              : perror("Unknown Error !!!\n");
         break ;
   }
   return 0;
}

// *************************************************************************************************************

int v1718::GetIrqStatus(void)
{
   CVErrorCodes	ret ;

   printf("CHECK IRQ\n");
   CAENVME_IRQCheck(Handle,&IrqStatus);		// Check IRQ Status

   printf("IRQ status: %i\n",IrqStatus);

   if(IrqStatus & (1<<(Level-1)))
   {
      ret = CAENVME_IACKCycle(Handle,Level,&Data,DataSize);
      switch (ret)
       {
          case cvSuccess    : printf("Cycle completed normally\n");
             printf("Int. Ack. on IRQ : %x  : StatusID = %d\n",Level,Data);
             break ;
          case cvBusError   : perror("Bus Error !!!\n");
             break ;
          case cvCommError  : perror("Communication Error !!!\n");
             break ;
          default           : perror("Unknown Error !!!\n");
             break ;

       }
   }
   else
      printf("Interrupt request IRQ %x not active\n",Level);
   return 0;
}
