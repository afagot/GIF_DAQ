// ****************************************************************************************************
// *   v1190a
// *   Alexis Fagot
// *   20/01/2015
// *   Based on v1290a file from :
// *   Avia Raviv & Y. Benhammou
// *   14/03/2011
// *
// *   Documentation about this VME TDC module can be found at the followin url
// *   http://www.caen.it/servlet/checkCaenManualFile?Id=8657
// *
// *   Comments will often refer to this user manual
// ****************************************************************************************************
#ifndef __LINUX
#define __LINUX
#endif

#include "CAENVMEtypes.h"

#ifndef _vme_h
#define _vme_h

class vme
{
    private :
        long                            Handle;

        Data32                          Data ;          	// Data
        CVIRQLevels		                Level ;             // Interrupt level
        CVAddressModifier	            AM ;			    // Addressing Mode
        CVDataWidth 		            DataSize ;          // Data Format
        Data32                          BaseAddress ;		// Base Address
        CVErrorCodes                    Status;             // Error code returned

    public:
        vme();
        ~vme();
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
        int SetLevel(CVIRQLevels level);
        CVIRQLevels GetLevel(void);
        long GetHandle(void);
        bool CheckIRQ(Data32 level);
        int WriteToVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize) ;
        int ReadFromVME(Data32 address, Data32 data,CVAddressModifier am,CVDataWidth dtsize) ;
};

#endif // _vme_h
