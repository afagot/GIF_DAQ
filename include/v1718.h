// **********************************************************************
// *   v1718 
// *   y.benhammou
// *   26/07/09
// **********************************************************************
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

#define BASEV1718 0x000000

class v1718
{
	private : 
        int32_t                         Handle;
        short 	 		                Device;
		
        Data32                          Address ;			// Address
        Data32                          Data ;          	// Data
        CVIRQLevels		                Level ;             // Interrupt level
        Data8                           IrqStatus;          // IRQ status
        CVAddressModifier	            AM ;			    // Addressing Mode
        CVDataWidth 		            DataSize ;          // Data Format
        Data32                          BaseAddress ;		// Base Address
        Data32                          BlockSize ;         // Block size for blt (bytes)
        Data16                          NbCycles ;          // Number of cycles
        CVErrorCodes                    Status;

		
	public:
        v1718(IniFile * inifile);
        ~v1718();
        Data8 ReadChar( Data32 address );
        Data16 ReadShort( Data32 address );
        Data32 ReadLong( Data32 address );
        void WriteChar( Data32 address, Data8 data );
        void WriteShort( Data32 address, Data16 data );
        void WriteLong( Data32 address, Data32 data );
        CVErrorCodes GetStatus(void);
        string GetError(void);
        int SetAddress(Data16 address);
        Data16 GetAddress(void);
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
		int GetIrqStatus(void) ;
};		
#endif
