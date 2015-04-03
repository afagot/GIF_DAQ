// **********************************************************************************
// *   v1718
// *   Alexis Fagot
// *   20/01/2015
// *   Based on v1718 file from :
// *   Y. Benhammou
// *   26/07/09
// *
// *   Documentation about this VME TDC module can be found at the following url
// *   http://www.caen.it/servlet/checkCaenManualFile?Id=5385
// *
// *   Comments will often refer to this user manual
// *
// *   This header file declares the class v1718 that controls the USB VME
// *   interface of the same name. A lot of private members and public method
// *   using them are implemented but not necessarily used.
// **********************************************************************************

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

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "IniFile.h"

using namespace std;

#ifndef _v1718_h
#define _v1718_h

#define BASEV1718 0x000000              //Page 32 Fig.3.3
                                        //The address is given by 2 rotary
                                        //switches where hexadecimal digits
                                        //are written. The position of the
                                        //switches gives you the base address.
                                        //Good example is given in the v1190a
                                        //users manual at page 35.


class v1718
{
    private :
        int32_t                         Handle;
        short 	 		                Device;

        Data32                          Data ;          	// Data
        CVIRQLevels		                Level ;             // Interrupt level
        Data8                           IrqStatus;          // IRQ status
        CVAddressModifier	            AM ;			    // Addressing Mode
        CVDataWidth 		            DataSize ;          // Data Format
        Data32                          BaseAddress ;		// Base Address
        Data32                          BlockSize ;         // Block size for blt (bytes)
        Data16                          NbCycles ;          // Number of cycles
        CVErrorCodes                    Status;             // Error code returned


    public:
        v1718(IniFile *inifile);
        ~v1718();
        Data8 ReadChar( Data32 address );
        Data16 ReadShort( Data32 address );
        Data32 ReadLong( Data32 address );
        void WriteChar( Data32 address, Data8 data );
        void WriteShort( Data32 address, Data16 data );
        void WriteLong( Data32 address, Data32 data );
        CVErrorCodes GetStatus(void);
        string GetError(void);
        int SetData(Data16 data);
        Data16 GetData(void);
        int SetAM(CVAddressModifier am);
        CVAddressModifier GetAM(void);
        int SetDatasize(CVDataWidth datasize);
        CVDataWidth GetDataSize(void);
        int SetBaseAddress(Data16 baseaddress);
        Data16 GetBaseAddress(void);
        int SetBlockSize(Data16 blocksize);
        Data16 GetBlockSize(void);
        int SetNbCycles(Data16 nbcycles);
        Data16 GetNbCycles(void);
        int SetLevel(CVIRQLevels level);
        CVIRQLevels GetLevel(void);
        int SetDevice(Data16 device);
        Data16 GetDevice(void);
        long GetHandle(void);
        int WriteToVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize) ;
        int ReadFromVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize) ;
        CVErrorCodes CheckIrqStatus(void);
};
#endif
