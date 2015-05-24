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
#include "IniFile.h"

using namespace std;

#ifndef _v1190a_h
#define _v1190a_h


// ****************************************************************************************************

/*** Base address (A32) of the model V1190A ***/

#define BASEV1190A                          0xEEEE0000

// ****************************************************************************************************

/*** Address map for the model V1190A ***/

#define ADD_OUT_BUFFER_V1190A				0x0000			/* Output buffer */
#define ADD_STATUS_V1190A					0x1002			/* Status register */
#define ADD_INT_LEVEL_V1190A				0x100A			/* Interrupt level register */
#define ADD_MOD_RESET_V1190A				0x1014			/* Module reset register */
#define ADD_SW_CLEAR_V1190A					0x1016			/* Software clear register */
#define ADD_EVENT_STORED_V1190A				0x1020			/* Event stored register */
#define ADD_ALMOST_FULL_LVL_V1190A			0x1022			/* Almost full level register */
#define ADD_BLT_EVENT_NUM_V1190A			0x1024			/* BLT event number register */
#define ADD_MICRO_V1190A					0x102E			/* Micro register */
#define ADD_MICRO_HND_V1190A				0x1030			/* Micro handshake register */

// ****************************************************************************************************

#define STATUS_TDC_V1190A                   0xF8000000

#define GLOBAL_HEADER_V1190A                0x40000000
#define GLOBAL_TRAILER_V1190A               0x80000000
#define GLOBAL_TRIGGER_TIME_TAG_V1190A      0x88000000

#define TDC_HEADER_V1190A                   0x08000000
#define TDC_DATA_V1190A                     0x00000000
#define TDC_ERROR_V1190A                    0x20000000
#define TDC_TRAILER_V1190A                  0x18000000

#define WRITE_OK                            0X0001
#define READ_OK                             0X0002
#define TEST_WR                             0xBEEF

#define TRIG_WIN_WIDTH_V1990A               0x50
#define TRIG_WIN_OFFSET_V1190A             -0x28
#define TRIG_SRCH_MARGIN_V1190A             0x04
#define TRIG_REJ_MARGIN_V1190A              0x04

#define MAXTRIGGERS_V1190A                  1000

#define BLOCK_SIZE                          10240

// ****************************************************************************************************

/*** Micro register opcodes ***/

typedef enum
{
    // Micro register opcodes: ACQUISITION MODE
     OPCODE_TRG_MATCH_V1190A				= 0x0000,		/* Trigger matching mode */

    // Micro register opcodes: TRIGGER
     OPCODE_SET_WIN_WIDTH_V1190A			= 0x1000,		/* Set window width */
     OPCODE_SET_WIN_OFFSET_V1190A			= 0x1100,		/* Set window offset */
     OPCODE_SET_SW_MARGIN_V1190A			= 0x1200,		/* Set extra search margin */
     OPCODE_SET_REJ_MARGIN_V1190A			= 0x1300,		/* Set reject margin */
     OPCODE_EN_SUB_TRG_V1190A				= 0x1400,		/* Enable subtraction of trigger time */
     OPCODE_DIS_SUB_TRG_V1190A				= 0x1500,		/* Disable subtraction of trigger time */
     OPCODE_READ_TRG_CONF_V1190A			= 0x1600,		/* Read trigger configuration */

    // Micro register opcodes: TDC EDGE DETECTION & RESOLUTION
     OPCODE_SET_DETECTION_V1190A			= 0x2200,		/* Enable paired meas. leading/trailing edge */
     OPCODE_READ_DETECTION_V1190A			= 0x2300,		/* Read edge detection configuration */
     OPCODE_SET_TR_LEAD_LSB_V1190A			= 0x2400,		/* Set LSB of leading/trailing edge */
     OPCODE_READ_RES_V1190A					= 0x2600,		/* Read resolution */
     OPCODE_SET_DEAD_TIME_V1190A			= 0x2800,		/* Set channel dead time between hits */
     OPCODE_READ_DEAD_TIME_V1190A			= 0x2900,		/* Read channel dead time between hits */

    // Micro register opcodes: TDC READOUT
     OPCODE_EN_HEAD_TRAILER_V1190A			= 0x3000,		/* Enable TDC header and trailer */
     OPCODE_DIS_HEAD_TRAILER_V1190A			= 0x3100,		/* Disable TDC header and trailer */
     OPCODE_READ_HEAD_TRAILER_V1190A		= 0x3200,		/* Read status TDC header and trailer */
     OPCODE_SET_EVENT_SIZE_V1190A			= 0x3300,		/* Set maximum number of hits per event */
     OPCODE_READ_EVENT_SIZE_V1190A			= 0x3400,		/* Read maximum number of hits per event */

    // Micro register opcodes: CHANNEL ENABLE
     OPCODE_WRITE_EN_PATTERN_V1190A			= 0x4400,		/* Write enable pattern for channels */
     OPCODE_READ_EN_PATTERN_V1190A			= 0x4500,		/* Read enable pattern for channels */

    // Micro register opcodes: DEBUG AND TEST
     OPCODE_READ_SPARE_V1190A				= 0xC400,		/* Read a 16 bit spare variable */
     OPCODE_EN_TEST_MODE_V1190A				= 0xC500,		/* Enable TDC test mode */
     OPCODE_DIS_TEST_MODE_V1190A			= 0xC600,		/* Disable TDC test mode */
}  OPCODES_MICRO_V1190A;

// ****************************************************************************************************

/*** Parameters ***/

typedef enum _EdgeMode {
    EdgeMode_Pair                             = 0b00,
    EdgeMode_Trailing                         = 0b01,
    EdgeMode_Leading                          = 0b10,
    EdgeMode_Both                             = 0b11

} EdgeMode;

typedef enum _Resolution {
    Res_800ps                                 = 0b00,
    Res_200ps                                 = 0b01,
    Res_100ps                                 = 0b10
} Resolution;

typedef enum _DeadTime {
    DT_5ns                                    = 0b00,
    DT_10ns                                   = 0b01,
    DT_30ns                                   = 0b10,
    DT_100ns                                  = 0b11
} DeadTime;

typedef enum _HitMax {
    HITS_0                                    = 0b0000,
    HITS_1                                    = 0b0001,
    HITS_2                                    = 0b0010,
    HITS_4                                    = 0b0011,
    HITS_8                                    = 0b0100,
    HITS_16                                   = 0b0101,
    HITS_32                                   = 0b0110,
    HITS_64                                   = 0b0111,
    HITS_128                                  = 0b1000,
    HITS_UNLIMITED                            = 0b1001       /* No limit on hits. */
} HitMax;

typedef enum _SetMode {
    ENABLE                                    = 1,
    DISABLE                                   = 0
} SetMode;

// ****************************************************************************************************

/*** CLASS ***/

class v1190a
{
 private :
    long                Handle;
    Data32              Address;
    CVDataWidth         DataWidth;
    CVAddressModifier   AddressModifier;

 public:

    v1190a(long handle, IniFile *inifile);
    ~v1190a();
    Data16              write_op_reg(Data32 address, int code);
    Data16              read_op_reg(Data32 address);
    void                Reset();
    void                Clear();
    void                TestWR(Data16 value);
    void                CheckStatus();
    void                CheckCommunication();
    void                SetTDCTestMode(Data16 mode);
    void                SetTrigMatching();
    void                SetTrigTimeSubstraction(Data16 mode);
    void                SetTrigWindowWidth(long  windowWidth);
    void                SetTrigWindowOffset(long windowOffset);
    void                SetTrigSearchMargin(long searchMargin);
    void                SetTrigRejectionMargin(long rejectMargin);
    void                SetTrigConfiguration(IniFile *inifile);
    void                GetTrigConfiguration();
    void                SetTDCDetectionMode(Data16 mode);
    void                SetTDCResolution(Data16 lsb);
    void                SetTDCDeadTime(Data16 time);
    void                SetTDCHeadTrailer(Data16 mode);
    void                SetTDCEventSize(Data16 size);
    void                SwitchChannels(IniFile *inifile);
    void                SetIRQ(Data32 level, Data32 count);
    void                SetBlockTransferMode(Data16 mode);
    void                Set(IniFile *inifile);
    int                 ReadBlockD32(const Data16 address, Data32 *data, const int words, bool ignore_berr);
    Uint                Read(string outputfilename);
};


#endif
