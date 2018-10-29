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

#ifndef _v1190a_h
#define _v1190a_h

#include "CAENVMEtypes.h"
#include "utils.h"
#include "IniFile.h"
#include "v1718.h"

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TString.h>
#include <string>
#include <map>

using namespace std;


// ****************************************************************************************************

/*** Base address (A32) of the model V1190A ***/

const Uint MINNTDC = 1;
const Data32 BASEV1190A = 0x11110000;

// ****************************************************************************************************

/*** Words used to test communications with TDCs ***/

typedef enum _COMMUNICATIONS_V1190A{
    WRITE_OK   = 0X0001,
    READ_OK    = 0X0002,
    TEST_WR    = 0xBEEF,
    READ_SPARE = 0x5555
} COMMUNICATIONS_V1190A;

// ****************************************************************************************************

/*** Address map for the model V1190A ***/

typedef enum _ADDRESS_MAP_V1190A{
    ADD_OUT_BUFFER_V1190A      = 0x0000,    /* Output buffer */
    ADD_STATUS_V1190A          = 0x1002,    /* Status register */
    ADD_INT_LEVEL_V1190A       = 0x100A,    /* Interrupt level register */
    ADD_MOD_RESET_V1190A       = 0x1014,    /* Module reset register */
    ADD_SW_CLEAR_V1190A        = 0x1016,    /* Software clear register */
    ADD_EVENT_STORED_V1190A    = 0x1020,    /* Event stored register */
    ADD_ALMOST_FULL_LVL_V1190A = 0x1022,    /* Almost full level register */
    ADD_BLT_EVENT_NUM_V1190A   = 0x1024,    /* BLT event number register */
    ADD_MICRO_V1190A           = 0x102E,    /* Micro register */
    ADD_MICRO_HND_V1190A       = 0x1030,    /* Micro handshake register */
    ADD_DUMMY16_V1190A         = 0x1204     /* Dummy 16bit */
} ADDRESS_MAP_V1190A;

// ****************************************************************************************************

/*** Micro register opcodes ***/

typedef enum _OPCODES_MICRO_V1190A{
    // Micro register opcodes: ACQUISITION MODE
    OPCODE_TRG_MATCH_V1190A         = 0x0000,    /* Trigger matching mode */

    // Micro register opcodes: TRIGGER
    OPCODE_SET_WIN_WIDTH_V1190A     = 0x1000,    /* Set window width */
    OPCODE_SET_WIN_OFFSET_V1190A    = 0x1100,    /* Set window offset */
    OPCODE_SET_SW_MARGIN_V1190A     = 0x1200,    /* Set extra search margin */
    OPCODE_SET_REJ_MARGIN_V1190A    = 0x1300,    /* Set reject margin */
    OPCODE_EN_SUB_TRG_V1190A        = 0x1400,    /* Enable subtraction of trigger time */
    OPCODE_DIS_SUB_TRG_V1190A       = 0x1500,    /* Disable subtraction of trigger time */
    OPCODE_READ_TRG_CONF_V1190A     = 0x1600,    /* Read trigger configuration */

    // Micro register opcodes: TDC EDGE DETECTION & RESOLUTION
    OPCODE_SET_DETECTION_V1190A     = 0x2200,    /* Enable paired meas. leading/trailing edge */
    OPCODE_READ_DETECTION_V1190A    = 0x2300,    /* Read edge detection configuration */
    OPCODE_SET_TR_LEAD_LSB_V1190A   = 0x2400,    /* Set LSB of leading/trailing edge */
    OPCODE_READ_RES_V1190A          = 0x2600,    /* Read resolution */
    OPCODE_SET_DEAD_TIME_V1190A     = 0x2800,    /* Set channel dead time between hits */
    OPCODE_READ_DEAD_TIME_V1190A    = 0x2900,    /* Read channel dead time between hits */

    // Micro register opcodes: TDC READOUT
    OPCODE_EN_HEAD_TRAILER_V1190A   = 0x3000,    /* Enable TDC header and trailer */
    OPCODE_DIS_HEAD_TRAILER_V1190A  = 0x3100,    /* Disable TDC header and trailer */
    OPCODE_READ_HEAD_TRAILER_V1190A = 0x3200,    /* Read status TDC header and trailer */
    OPCODE_SET_EVENT_SIZE_V1190A    = 0x3300,    /* Set maximum number of hits per event */
    OPCODE_READ_EVENT_SIZE_V1190A   = 0x3400,    /* Read maximum number of hits per event */

    // Micro register opcodes: CHANNEL ENABLE
    OPCODE_WRITE_EN_PATTERN_V1190A  = 0x4400,    /* Write enable pattern for channels */
    OPCODE_READ_EN_PATTERN_V1190A   = 0x4500,    /* Read enable pattern for channels */

    // Micro register opcodes: DEBUG AND TEST
    OPCODE_READ_SPARE_V1190A        = 0xC400,    /* Read a 16 bit spare variable */
    OPCODE_EN_TEST_MODE_V1190A      = 0xC500,    /* Enable TDC test mode */
    OPCODE_DIS_TEST_MODE_V1190A     = 0xC600     /* Disable TDC test mode */
}  OPCODES_MICRO_V1190A;

// ****************************************************************************************************

/*** Identifiers for the output buffer headers and trailers ***/

typedef enum _OUTPUT_BUFFER_HEAD_TRAIL_V1190A{
    STATUS_TDC_V1190A              = 0xF8000000, /* Output buffer header/trailer identifier bits[31,27] */
    GLOBAL_HEADER_V1190A           = 0x40000000, /* Output buffer global header bits[31,27] = 01000 */
    GLOBAL_HEADER_EVT_COUNT_V1190A = 0x07FFFFE0, /* Output buffer global header bits event count [26,5] */
    GLOBAL_TRAILER_V1190A          = 0x80000000, /* Output buffer global trailer bits[31,27] = 10000 */
    GLOBAL_TRIGGER_TIME_TAG_V1190A = 0x88000000, /* Output buffer global trigger time tag bits[31,27] = 10001 */
    TDC_DATA_V1190A                = 0x00000000, /* Output buffer TDC data bits[31,27] = 00XXX */
    TDC_HEADER_V1190A              = 0x08000000, /* Output buffer TDC header bits[29,27] = 001 */
    TDC_MEASUR_V1190A              = 0x00000000, /* Output buffer TDC measurement bits[29,27] = 000 */
    TDC_MEASUR_EDGE_V1190A         = 0x04000000, /* Output buffer TDC measurement edge type bit[26] */
    TDC_MEASUR_CHAN_V1190A         = 0x03F80000, /* Output buffer TDC measurement channel bits[25,19] */
    TDC_MEASUR_TIME_V1190A         = 0x0007FFFF, /* Output buffer TDC measurement time bit[18,0] */
    TDC_ERROR_V1190A               = 0x20000000, /* Output buffer TDC error bits[29,27] = 100 */
    TDC_TRAILER_V1190A             = 0x18000000  /* Output buffer TDC trailer bits[29,27] = 011 */
} OUTPUT_BUFFER_HEAD_TRAIL_V1190A;

// ****************************************************************************************************

/*** Default TDC trigger settings ***/

const Uint CLOCK = 25;
const Uint MAXOFFSET = 65536;

typedef enum _TRIGGER_DEFAULT_SETTINGS_V1190A{
    TRIG_DEF_WIDTH_V1990A   =   40, /* Default window width */
    TRIG_DEF_OFFSET_V1190A  =  -41, /* Default window offset */
    TRIG_EFF_WIDTH_V1990A   =   24, /* Default window width in efficiency runs */
    TRIG_EFF_OFFSET_V1190A  =  -29, /* Default window offset in efficiency runs */
    TRIG_RATE_WIDTH_V1990A  =  400, /* Default window width in rate runs */
    TRIG_RATE_OFFSET_V1190A = -401, /* Default window offset in rate runs */
    TRIG_SRCH_MARGIN_V1190A = 0x04, /* Default extra search margin */
    TRIG_REJ_MARGIN_V1190A  = 0x04  /* Default reject margin */
} TRIGGER_DEFAULT_SETTINGS_V1190A;

const Uint MAXTRIGGERS_V1190A = 1000;
const Uint TIMEOUT = 100000;
const Uint BLOCK_SIZE = 100;
const Uint IRQ_BUFFER = 4096;

// ****************************************************************************************************

/*** Parameters ***/

typedef enum _EdgeMode {
    EdgeMode_Pair     = 0b00,
    EdgeMode_Trailing = 0b01,
    EdgeMode_Leading  = 0b10,
    EdgeMode_Both     = 0b11,
    Check_Edge        = 0b11
} EdgeMode;

const map<Uint,string> EdgeModeMap{
    {EdgeMode_Pair,"Pair"},
    {EdgeMode_Trailing,"Trailing"},
    {EdgeMode_Leading,"Leading"},
    {EdgeMode_Both,"Both"}
};

typedef enum _EdgeMeasurement {
    TRAILING  = 1,
    LEADING = 0
} EdgeMeasurement;

typedef enum _Resolution {
    Res_800ps = 0b00,
    Res_200ps = 0b01,
    Res_100ps = 0b10,
    Check_Res = 0b11
} Resolution;

const map<Uint,string> ResolutionMap{
    {Res_800ps,"800ps"},
    {Res_200ps,"200ps"},
    {Res_100ps,"100ps"}
};

typedef enum _DeadTime {
    DT_5ns   = 0b00,
    DT_10ns  = 0b01,
    DT_30ns  = 0b10,
    DT_100ns = 0b11,
    Check_DT = 0b11
} DeadTime;

const map<Uint,string> DeadTimeMap{
    {DT_5ns,"5ns"},
    {DT_10ns,"10ns"},
    {DT_30ns,"30ns"},
    {DT_100ns,"100ns"}
};

typedef enum _HitMax {
    HITS_0         = 0b0000,
    HITS_1         = 0b0001,
    HITS_2         = 0b0010,
    HITS_4         = 0b0011,
    HITS_8         = 0b0100,
    HITS_16        = 0b0101,
    HITS_32        = 0b0110,
    HITS_64        = 0b0111,
    HITS_128       = 0b1000,
    HITS_UNLIMITED = 0b1001,
    Check_HITS     = 0b1111
} HitMax;

const map<Uint,string> HitMaxMap{
    {HITS_0,"0"},
    {HITS_1,"1"},
    {HITS_2,"2"},
    {HITS_4,"4"},
    {HITS_8,"8"},
    {HITS_16,"16"},
    {HITS_32,"32"},
    {HITS_64,"64"},
    {HITS_128,"64"},
    {HITS_UNLIMITED,"Unlimited"}
};

typedef enum _SetMode {
    ENABLE  = 1,
    DISABLE = 0,
    CheckMode = 0b1
} SetMode;

const map<Uint,string> SetModeMap{
    {ENABLE,"Enabled"},
    {DISABLE,"Disabled"}
};

typedef enum _QualityFlag {
    GOOD      = 1,
    CORRUPTED = 0
} QualityFlag;

const Uint Data16HexStringLength = 4; //16 channels per connector
const Uint Check_Channels = 0xFFFF;

// ****************************************************************************************************

/*** STRUCTURE ***/

struct RAWData{
    vector<int>            *EventList;
    vector<int>            *NHitsList;
    vector<int>            *QFlagList;
    vector<vector<int> >   *ChannelList;
    vector<vector<float> > *LeadEdgeTSList;
    vector<vector<float> > *TrailEdgeTSList;
};

/*** CLASS ***/

class v1190a
{
 private :
    long              Handle;
    vector<Data32>    Address;
    CVDataWidth       DataWidth;
    CVAddressModifier AddressModifier;

 public:

    v1190a(long handle, IniFile *inifile, int ntdcs);
    ~v1190a();
    Data16 write_op_reg(Data32 address, int code, string error);
    Data16 read_op_reg(Data32 address, string error);
    void   Reset(int ntdcs);
    void   Clear(int ntdcs);
    void   TestWR(Data16 value,int ntdcs);
    void   CheckTDCStatus(int ntdcs);
    void   CheckCommunication(int ntdcs);
    void   SetTDCTestMode(Data16 mode,int ntdcs);
    void   SetTrigMatching(int ntdcs);
    void   SetTrigTimeSubstraction(Data16 mode,int ntdcs);
    void   SetTrigWindowWidth(Uint windowWidth,int ntdcs);
    void   SetTrigWindowOffset(Uint windowOffset,int ntdcs);
    void   SetTrigSearchMargin(Uint searchMargin,int ntdcs);
    void   SetTrigRejectionMargin(Uint rejectMargin,int ntdcs);
    void   GetTrigConfiguration(int ntdcs);
    void   SetTrigConfiguration(IniFile *inifile,int ntdcs);
    void   SetTDCDetectionMode(Data16 mode,int ntdcs);
    void   SetTDCResolution(Data16 lsb,int ntdcs);
    void   SetTDCDeadTime(Data16 time,int ntdcs);
    void   SetTDCHeadTrailer(Data16 mode,int ntdcs);
    void   SetTDCEventSize(Data16 size,int ntdcs);
    void   SwitchChannels(IniFile *inifile,int ntdcs);
    void   SetIRQ(Data32 level, Data32 count,int ntdcs);
    void   SetBlockTransferMode(Data16 mode,int ntdcs);
    void   Set(IniFile *inifile,int ntdcs);
    void   CheckStatus(CVErrorCodes status) const;
    int    ReadBlockD32(Uint tdc, const Data16 address, Data32 *data, const Uint words, bool ignore_berr);
    Uint   Read(RAWData *DataList,int ntdcs);
};


#endif
