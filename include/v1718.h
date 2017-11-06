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

#ifndef _v1718_h
#define _v1718_h

#include "CAENVMEtypes.h"
#include "IniFile.h"

using namespace std;

#define BASEV1718 0xFF0000    //Page 32 Fig.3.3
                              //The address is given by 2 rotary
                              //switches where hexadecimal digits
                              //are written. The position of the
                              //switches gives you the base address.
                              //Good example is given in the v1190a
                              //users manual at page 35.

typedef enum _BusyLevel {
    ON  = 1,
    OFF = 0
} BusyLevel;


class v1718{

    private :
        int               Handle;

        Data32            Data;           // Data
        CVIRQLevels       Level;          // Interrupt level
        CVAddressModifier AM;             // Addressing Mode
        CVDataWidth 	  DataSize;       // Data Format
        Data32            BaseAddress;    // Base Address

    public:
        v1718(IniFile *inifile);
        ~v1718();

        long              GetHandle(void) const;
        int               SetData(Data16 data);
        Data16            GetData(void);
        int               SetLevel(CVIRQLevels level);
        CVIRQLevels       GetLevel(void);
        int               SetAM(CVAddressModifier am);
        CVAddressModifier GetAM(void);
        int               SetDatasize(CVDataWidth datasize);
        CVDataWidth       GetDataSize(void);
        int               SetBaseAddress(Data16 baseaddress);
        Data16            GetBaseAddress(void);
        void              CheckStatus(CVErrorCodes status) const;
        bool              CheckIRQ();
        void              SetPulsers();
        void              SendBUSY(BusyLevel level);
};
#endif
