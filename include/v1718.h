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
#include "utils.h"
#include "IniFile.h"

using namespace std;

#define BASEV1718 0xFF0000    //Page 32 Fig.3.3
                              //The address is given by 2 rotary
                              //switches where hexadecimal digits
                              //are written. The position of the
                              //switches gives you the base address.
                              //Good example is given in the v1190a
                              //users manual at page 35.

typedef enum{
    //Clear register
    CLEAR0 = 0x0403,
    CLEAR1 = 0x080C,
    CLEAR2 = 0x1030,
    CLEAR3 = 0x20C0,
    CLEAR4 = 0x4300,
    CLEARALL = 0x7FFF,

    //Set Default : Data Strobe, Address Strobe, Data Acknowledge, Bus Error and Location Monitor
    SETDEFAULT = 0x0000,

    //Set Input 0 AND Input 0
    SET0AND = 0x0001,
    SET1AND = 0x0004,
    SET2AND = 0x0010,
    SET3AND = 0x0040,
    SET4AND = 0x0100,
    SETALLAND = 0x0155,

    //Set Pulsers and Scaler
    SET0PULS = 0x0002,
    SET1PULS = 0x0008,
    SET2PULS = 0x0020,
    SET3PULS = 0x0080,
    SET4SCA = 0x0200,
    SETALLPULS = 0x02AA,

    //Set output register
    SET0REG = 0x0003,
    SET1REG = 0x000C,
    SET2REG = 0x0030,
    SET3REG = 0x00C0,
    SET4REG = 0x0300,
    SETALLREG = 0x03FF,

    //Set Inverted signals
    SET0INV = 0x0400,
    SET1INV = 0x0800,
    SET2INV = 0x1000,
    SET3INV = 0x2000,
    SET4INV = 0x4000,
    SETALLINV = 0x7C00
} OUTPUT_MUX_REG_CODES;

typedef enum _PulserLevel {
    ON  = 1,
    OFF = 0
} PulserLevel;

const Uint BUSY_WIDTH = 1;
const Uint RDM_PULSER_FRQ = 100;

const map<CVTimeUnits, float> StepUnitMap{
    {cvUnit25ns,25e-9},
    {cvUnit1600ns,1.6e-6},
    {cvUnit410us,410e-6},
    {cvUnit104ms,104e-3}
};

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
        void              SetPulsers(Uint RDM_Frequency);
        void              SendBUSY(PulserLevel level);
        void              RDMTriggerPulse(PulserLevel level);
};
#endif
