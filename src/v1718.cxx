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
// *
// *   This file declares defines the public methods of the v1718 class. A lot
// *   of methods are available but not used.
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

v1718::v1718(IniFile *inifile){
   MSG_INFO("Initialization VME...");

   //Get the base address from the configuration file
   Data32 baseaddress = inifile->addressType("VMEInterface","BaseAddress",BASEV1718);

   //Initialisation of the module. See CAENVMElib.h & CAENVMEtypes.h .
   if( CAENVME_Init(cvV1718, baseaddress, 0, &Handle) != cvSuccess ){
      perror("\n\nError opening the v1718 controller device\n");
      exit(1);
   }

   //Enable FIFO mode. See CAENVMElib.h .
   CAENVME_SetFIFOMode(Handle,1);


   this->SetLevel(cvIRQ1);
   this->SetAM(cvA24_U_DATA);
   this->SetDatasize(cvD16);
   this->SetBaseAddress(baseaddress);
   this->SetBlockSize(256);
   this->SetNbCycles(1);
   MSG_INFO(" OK\n\n");
}

// *************************************************************************************************************

v1718::~v1718(){
    CAENVME_End(Handle);
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_ReadCycle function for
// 8-bits long data. See CAENVMElib.h .

Data8 v1718::ReadChar( Data32 address ){
    Data8 Data;
    Status = CAENVME_ReadCycle(Handle, address, &Data, cvA32_U_DATA, cvD8 );
    return Data;
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_ReadCycle function for
// 16-bits long data. See CAENVMElib.h .

Data16 v1718::ReadShort( Data32 address ){
    Data16 Data;
    Status = CAENVME_ReadCycle( Handle, address, &Data, cvA32_U_DATA, cvD16 );
    return Data;
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_ReadCycle function for
// 32-bits long data. See CAENVMElib.h .

Data32 v1718::ReadLong( Data32 address ){
    Data32 Data;
    Status = CAENVME_ReadCycle( Handle, address, &Data, cvA32_U_DATA, cvD32 );
    return Data;
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_WriteCycle function for
// 8-bits long data. See CAENVMElib.h .

void v1718::WriteChar( Data32 address, Data8 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD8 );
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_WriteCycle function for
// 16-bits long data. See CAENVMElib.h .

void v1718::WriteShort( Data32 address, Data16 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD16 );
}

// *************************************************************************************************************
// This function is an implementation of the CAENVME_WriteCycle function for
// 32-bits long data. See CAENVMElib.h .

void v1718::WriteLong( Data32 address, Data32 data ){
    Status = CAENVME_WriteCycle( Handle, address, &data, cvA32_U_DATA, cvD32 );
}

// *************************************************************************************************************
// Get the error code returned by the latest Read or Write used.
CVErrorCodes v1718::GetStatus(void)
{
  return Status;
}

// *************************************************************************************************************
// As the name says, it decodes the error code previously returned and saved
// into Status. Returns a string describing the error code. See CAEMVMElib.h .

string v1718::GetError(void){
    return CAENVME_DecodeError(Status);
}

// *************************************************************************************************************
// Data represents what you want to write or read from your VME bus (depending
// on which CAEN method you call

int v1718::SetData(Data16 data)
{
  Data=data;
  return 0;
}

// *************************************************************************************************************
// Return the value of Data. To use in case you read data from the VME bus
// using CAEN's methods.

Data16 v1718::GetData(void)
{
  return Data;
}

// *************************************************************************************************************
// See the list of address modifiers in the CVAddressModifier enum of
// CAENVMEtypes.h . More explanation on Page 34 (4.4.1) and Page 35 (4.4.3).

int v1718::SetAM(CVAddressModifier am)
{
  AM=am;
  return 0;
}

// *************************************************************************************************************
// Return the value of the address modifier

CVAddressModifier v1718::GetAM(void)
{
  return AM;
}

// *************************************************************************************************************
// As you can understand it, this specifies the data width you are using. A
// list is available in CVDataWidth enum of CAENVMEtypes.h .

int v1718::SetDatasize(CVDataWidth datasize)
{
  DataSize=datasize;
  return 0;
}

// *************************************************************************************************************
// Return the data width used.

CVDataWidth v1718::GetDataSize(void)
{
  return DataSize;
}

// *************************************************************************************************************
// Allows to set the base address of the VME interface. This is available at
// Page 32 (Fig.3.3) or on Page 35 of the V1190A users manual. A little
// explanation is also given in the header file v1718.h
.
int v1718::SetBaseAddress(Data16 baseaddress)
{
  BaseAddress=baseaddress;
  return 0;
}

// *************************************************************************************************************
// Return the base address of the vme interface.

Data16 v1718::GetBaseAddress(void)
{
  return BaseAddress;
}

// *************************************************************************************************************
// In the case you communicate with the VME interface using Chain Block
// Transfer (CBLT) readout instead of using the Base Address, you specify
// the block size. This is the same than specifying the data width when you
// use the Base Address.

int v1718::SetBlockSize(Data16 blocksize)
{
  BlockSize=blocksize;
  return 0;
}

// *************************************************************************************************************
// Return the block size.

Data16 v1718::GetBlockSize(void)
{
  return BlockSize;
}

// *************************************************************************************************************
// Number of block transfer cycles used to transfert the memory content of the
// module.

int v1718::SetNbCycles(Data16 nbcycles)
{
  NbCycles=nbcycles;
  return 0;
}

// *************************************************************************************************************
// Return the number of cycles.

Data16 v1718::GetNbCycles(void)
{
  return NbCycles;
}

// *************************************************************************************************************
// Set the Interrupt Request level. The IRQ Status register is a word of 7 bits
// each bit representing a level from IRQ1 to IRQ7.

int v1718::SetLevel(CVIRQLevels level)
{
  Level=level;
  return 0;
}

// *************************************************************************************************************
// Return the level of the eventual IRQs.

CVIRQLevels v1718::GetLevel(void)
{
  return Level;
}

// *************************************************************************************************************
// Set the device (not currently used).

int v1718::SetDevice(Data16 device)
{
  Device=device;
  return 0;
}

// *************************************************************************************************************
// Return the value of the device.

Data16 v1718::GetDevice(void)
{
  return Device;
}

// *************************************************************************************************************
// Return the handle that identifies the device.

long v1718::GetHandle(void)
{
  return Handle;
}

// *************************************************************************************************************
// Write into the VME bus. Could be implemented to make use of the private
// member Status and of the methods WriteChar, WriteShort & WriteLong. Here
// the message corresponding to the returned error code is printed.

int v1718::WriteToVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize)
{
   CVErrorCodes ret;

   if(dtsize == cvD64)
   {
      perror("Can't execute a D64 Write Cycle");
      return -1;
   }
   ret = CAENVME_WriteCycle(Handle,BaseAddress,&Data,AM,DataSize);

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
// Read the VME bus. Could be implemented to make use of the private member
// Status and of the methods ReadChar, ReadShort & ReadLong.  Here the message
// corresponding to the returned error code is printed.

int v1718::ReadFromVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize)
{
   CVErrorCodes ret;

   BaseAddress=address;
   Data=data;
   AM=am;
   DataSize=dtsize;

   if(dtsize == cvD64)
   {
      perror("Can't execute a D64 Read Cycle");
      return -1;
   }
   ret = CAENVME_ReadCycle(Handle,BaseAddress,&Data,AM,DataSize);

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
// Get the Interrupt Request Status and print whether they are active or not.

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
