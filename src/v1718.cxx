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

#include <cstdio>
#include <cstdlib>

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"

#include "../include/v1718.h"
#include "../include/MsgSvc.h"

using namespace std;

v1718::v1718(IniFile *inifile){
   MSG_INFO("v1718: \t Initialization VME bridge...\n");

   //Get the base address from the configuration file
   Data32 baseaddress = inifile->addressType("VMEInterface","BaseAddress",BASEV1718);

   //Initialisation of the module. See CAENVMElib.h & CAENVMEtypes.h
   try{
       CheckStatus(CAENVME_Init(cvV1718, baseaddress, 0, &Handle)) != cvSuccess )
   } catch(exception &e){
      MSG_ERROR("v1718: \t Error when trying to initialise the device\n");
      throw MSG_ERROR(e.what());
   }

   SetLevel(cvIRQ1);
   SetAM(cvA24_U_DATA);
   SetDatasize(cvD16);
   SetBaseAddress(baseaddress);

   MSG_INFO("v1718: \t OK\n");
}

// *************************************************************************************************************

v1718::~v1718(){
    CAENVME_End(Handle);
}

// *************************************************************************************************************
// Return the handle that identifies the device.

long v1718::GetHandle(void) const{
    return Handle;
}

// *************************************************************************************************************
// Data represents what you want to write or read from your VME bus (depending
// on which CAEN method you call

int v1718::SetData(Data16 data){
    Data=data;
    return 0;
}

// *************************************************************************************************************
// Return the value of Data. To use in case you read data from the VME bus
// using CAEN's methods.

Data16 v1718::GetData(void){
    return Data;
}

// *************************************************************************************************************
// See the list of address modifiers in the CVAddressModifier enum of
// CAENVMEtypes.h . More explanation on Page 34 (4.4.1) and Page 35 (4.4.3).

int v1718::SetAM(CVAddressModifier am){
    AM=am;
    return 0;
}

// *************************************************************************************************************
// Return the value of the address modifier

CVAddressModifier v1718::GetAM(void){
    return AM;
}

// *************************************************************************************************************
// As you can understand it, this specifies the data width you are using. A
// list is available in CVDataWidth enum of CAENVMEtypes.h .

int v1718::SetDatasize(CVDataWidth datasize){
    DataSize=datasize;
    return 0;
}

// *************************************************************************************************************
// Return the data width used.

CVDataWidth v1718::GetDataSize(void){
    return DataSize;
}

// *************************************************************************************************************
// Allows to set the base address of the VME interface. This is available at
// Page 32 (Fig.3.3) or on Page 35 of the V1190A users manual. A little
// explanation is also given in the header file v1718.h

int v1718::SetBaseAddress(Data16 baseaddress){
    BaseAddress=baseaddress;
    return 0;
}

// *************************************************************************************************************
// Return the base address of the vme interface.

Data16 v1718::GetBaseAddress(void){
    return BaseAddress;
}

// *************************************************************************************************************
// Set the Interrupt Request level. The IRQ Status register is a word of 7 bits
// each bit representing a level from IRQ1 to IRQ7.

int v1718::SetLevel(CVIRQLevels level){
    Level=level;
    return 0;
}

// *************************************************************************************************************
// Return the level of the eventual IRQs.

CVIRQLevels v1718::GetLevel(void){
    return Level;
}

// *************************************************************************************************************
// As the name says, it decodes the error code previously returned and saved
// into Status. Returns a string describing the error code. See CAEMVMElib.h .

void v1718::CheckStatus(CVErrorCodes status) const{
    // These exceptions will get passed up the call chain
    // This provides more flexible error handling, as the return value method is more of a C-ism
    switch (status){
        case cvBusError:
            throw MSG_ERROR("v1718: \t VME bus error");
        case cvCommError:
            throw MSG_ERROR("v1718: \t Communication error");
        case cvGenericError:
            throw MSG_ERROR("v1718: \t General VME library error");
        case cvInvalidParam:
            throw MSG_ERROR("v1718: \t Invalid parameter passed to VME library");
        case cvTimeoutError:
            throw MSG_ERROR("v1718: \t Request timed out");
        default:
            return;
    }
}

// *************************************************************************************************************
// Get the Interrupt Request Status and print whether they are active or not.

bool v1718::CheckIRQ(){
    CheckStatus(CAENVME_IRQCheck(Handle, &Data));

    // Pick the requested IRQ line from the data and return its status
    return (((Data>>Level-1) & 1) > 0);
}
