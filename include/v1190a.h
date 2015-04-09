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

#include "v1718.h"
#include "IniFile.h"

using namespace std;

#ifndef _v1190a_h
#define _v1190a_h



#define ERR -1

// ****************************************************************************************************

/*** Base address (A32) of the model V1190A ***/

#define BASEV1190A                          0xEEEE0000

// ****************************************************************************************************

/*** Address map for the model V1190A ***/

#define ADD_OUT_BUFFER_V1190A				0x0000			/* Output buffer */
#define ADD_CONTROL_V1190A					0x1000			/* Control register */
#define ADD_STATUS_V1190A					0x1002			/* Status register */
#define ADD_INT_LEVEL_V1190A				0x100A			/* Interrupt level register */
#define ADD_INT_VECTOR_V1190A				0x100C			/* Interrupt vector register */
#define ADD_GEO_V1190ARESS_V1190A			0x100E			/* Geo Address register */
#define ADD_MCST_CBLT_V1190ARESS_V1190A		0x1010			/* MCST/CBLT Base Address register */
#define ADD_MCST_CBLT_CTRL_V1190A			0x1012			/* MCST/CBLT Control register */
#define ADD_MOD_RESET_V1190A				0x1014			/* Module reset register */
#define ADD_SW_CLEAR_V1190A					0x1016			/* Software clear register */
#define ADD_SW_EVENT_RESET_V1190A			0x1018			/* Software event reset register */
#define ADD_SW_TRIGGER_V1190A				0x101A			/* Software trigger register */
#define ADD_EVENT_COUNTER_V1190A			0x101C			/* Event counter register */
#define ADD_EVENT_STORED_V1190A				0x1020			/* Event stored register */
#define ADD_ALMOST_FULL_LVL_V1190A			0x1022			/* Almost full level register */
#define ADD_BLT_EVENT_NUM_V1190A			0x1024			/* BLT event number register */
#define ADD_FW_REV_V1190A					0x1026			/* Firmware revision register */
#define ADD_TESTREG_V1190A					0x1028			/* Testreg register */
#define ADD_OUT_PROG_CTRL_V1190A			0x102C			/* Output prog control register */
#define ADD_MICRO_V1190A					0x102E			/* Micro register */
#define ADD_MICRO_HND_V1190A				0x1030			/* Micro handshake register */
#define ADD_SEL_FLASH_V1190A				0x1032			/* Select Flash register */
#define ADD_FLASH_V1190A					0x1034			/* Flash register */
#define ADD_COMP_SRAM_PAGE_V1190A			0x1036			/* Compensation SRAM Page register */
#define ADD_EVENT_FIFO_V1190A				0x1038			/* Event FIFO register */
#define ADD_EVENT_FIFO_STORED_V1190A		0x103C			/* Event FIFO stored register */
#define ADD_EVENT_FIFO_STATUS_V1190A		0x103E			/* Event FIFO status register */
#define ADD_DUMMY32_V1190A					0x1200			/* Dummy 32 register */
#define ADD_DUMMY16_V1190A					0x1204			/* Dummy 16 register */
#define ADD_ROM_OUI_2_V1190A				0x4024			/* Manufacturer identifier (IEEE OUI) (MSB) */
#define ADD_ROM_OUI_1_V1190A				0x4028			/* Manufacturer identifier (IEEE OUI) */
#define ADD_ROM_OUI_0_V1190A				0x402C			/* Manufacturer identifier (IEEE OUI) (LSB) */
#define ADD_ROM_VERSION_V1190A				0x4030			/* Purchased version of the Mod.V1190 */
#define ADD_ROM_BOARD_ID_2_V1190A			0x4034			/* Board identifier (MSB) */
#define ADD_ROM_BOARD_ID_1_V1190A			0x4038			/* Board identifier */
#define ADD_ROM_BOARD_ID_0_V1190A   		0x403C			/* Board identifier (LSB) */
#define ADD_ROM_REVISION_3_V1190A   		0x4040			/* Hardware revision identifier */
#define ADD_ROM_REVISION_2_V1190A   		0x4044			/* Hardware revision identifier */
#define ADD_ROM_REVISION_1_V1190A   		0x4048			/* Hardware revision identifier */
#define ADD_ROM_REVISION_0_V1190A   		0x404C			/* Hardware revision identifier */
#define ADD_ROM_SERIAL_1_V1190A	   			0x4080			/* Serial number (MSB) */
#define ADD_ROM_SERIAL_0_V1190A	   			0x4084			/* Serial number (LSB) */

#define NO_BUFFER_EMPTY_V1190A              0x0001

// ****************************************************************************************************

#define STATUS_TDC_V1190A                   0xF8000000

#define FIFO_WORDCOUNT_MASK_V1190A          0x0000FFFF
#define FIFO_WORDCOUNT_RSHIFT_V1190A        0x00000000
#define FIFO_EVENTCOUNT_MASK_V1190A         0xFFFF0000
#define FIFO_EVENTCOUNT_RSHIFT_V1190A       0x000000F1
#define TYPE_MASK_V1190A                    0xF8000000

#define FILLER_LONG_V1190A                  0xc0000000
#define GLOBAL_HEADER_V1190A                0x40000000
#define GLOBAL_TRAILER_V1190A               0x80000000
#define GLOBAL_TRIGGER_TIME_TAG_V1190A      0x88000000

#define TDC_HEADER_V1190A                   0x08000000
#define TDC_DATA_V1190A                     0x00000000
#define TDC_ERROR_V1190A                    0x20000000
#define TDC_TRAILER_V1190A                  0x18000000

#define WRITE_OK                            0X0001
#define READ_OK                             0X0002
#define EVENT_FIFO_DREADY_V1190A            0X01
#define EVENT_FIFO_FULL_V1190A              0X02

#define SIZE_V1190A                         0x7D00
#define MAXDATA_V1190A                      512

#define MAXDATA                             1024

#define TEST_WR                             365

#define TRIG_WIN_WIDTH_V1990A               0x50
#define TRIG_WIN_OFFSET_V1190A              -0x28
#define TRIG_SRCH_MARGIN_V1190A             0x04
#define TRIG_REJ_MARGIN_V1190A              0x04

#define MAXTRIGGERS_V1190A                  1000

// ****************************************************************************************************

/*** Micro register opcodes ***/

typedef enum
{
    // Micro register opcodes: ACQUISITION MODE
     OPCODE_TRG_MATCH_V1190A				= 0x0000,		/* Trigger matching mode */
     OPCODE_CONT_STORE_V1190A				= 0x0100,		/* Continuous storage */
     OPCODE_READ_ACQ_MOD_V1190A				= 0x0200,		/* Read acquisition mode */
     OPCODE_SET_KEEP_TOKEN_V1190A			= 0x0300,		/* Set keep_token */
     OPCODE_CLEAR_KEEP_TOKEN_V1190A			= 0x0400,		/* Clear keep_token */
     OPCODE_LOAD_DEF_CONFIG_V1190A			= 0x0500,		/* Load default configuration */
     OPCODE_SAVE_USER_CONFIG_V1190A			= 0x0600,		/* Save User configuration */
     OPCODE_LOAD_USER_CONFIG_V1190A			= 0x0700,		/* Load User configuration */
     OPCODE_AUTOLOAD_USER_CONFIG_V1190A		= 0x0800,		/* Set auto load User configuration */
     OPCODE_AUTOLOAD_DEF_CONFIG_V1190A		= 0x0900,		/* Set auto load default configuration */

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
     OPCODE_SET_PAIR_RES_V1190A				= 0x2500,		/* Set leading time and width res. when pair */
     OPCODE_READ_RES_V1190A					= 0x2600,		/* Read resolution */
     OPCODE_SET_DEAD_TIME_V1190A			= 0x2800,		/* Set channel dead time between hits */
     OPCODE_READ_DEAD_TIME_V1190A			= 0x2900,		/* Read channel dead time between hits */

    // Micro register opcodes: TDC READOUT
     OPCODE_EN_HEAD_TRAILER_V1190A			= 0x3000,		/* Enable TDC header and trailer */
     OPCODE_DIS_HEAD_TRAILER_V1190A			= 0x3100,		/* Disable TDC header and trailer */
     OPCODE_READ_HEAD_TRAILER_V1190A		= 0x3200,		/* Read status TDC header and trailer */
     OPCODE_SET_EVENT_SIZE_V1190A			= 0x3300,		/* Set maximum number of hits per event */
     OPCODE_READ_EVENT_SIZE_V1190A			= 0x3400,		/* Read maximum number of hits per event */
     OPCODE_EN_ERROR_MARK_V1190A			= 0x3500,		/* Enable TDC error mark */
     OPCODE_DIS_ERROR_MARK_V1190A			= 0x3600,		/* Disable TDC error mark */
     OPCODE_EN_ERROR_BYPASS_V1190A			= 0x3700,		/* Enable bypass TDC if error */
     OPCODE_DIS_ERROR_BYPASS_V1190A			= 0x3800,		/* Disable bypass TDC if error */
     OPCODE_SET_ERROR_TYPES_V1190A			= 0x3900,		/* Set TDC internal error type */
     OPCODE_READ_ERROR_TYPES_V1190A			= 0x3A00,		/* Read TDC internal error type */
     OPCODE_SET_FIFO_SIZE_V1190A			= 0x3B00,		/* Set effective size of readout FIFO */
     OPCODE_READ_FIFO_SIZE_V1190A			= 0x3C00,		/* Read effective size of readout FIFO */

    // Micro register opcodes: CHANNEL ENABLE
     OPCODE_EN_CHANNEL_V1190A				= 0x4000,		/* Enable channel nn */
     OPCODE_DIS_CHANNEL_V1190A				= 0x4100,		/* Disable channel nn */
     OPCODE_EN_ALL_CH_V1190A				= 0x4200,		/* Enable all channels */
     OPCODE_DIS_ALL_CH_V1190A				= 0x4300,		/* Disable all channels */
     OPCODE_WRITE_EN_PATTERN_V1190A			= 0x4400,		/* Write enable pattern for channels */
     OPCODE_READ_EN_PATTERN_V1190A			= 0x4500,		/* Read enable pattern for channels */
     OPCODE_WRITE_EN_PATTERN32_V1190A		= 0x4600,		/* Write 32 bit enable pattern for channels */
     OPCODE_READ_EN_PATTERN32_V1190A		= 0x4700,		/* Read 32 bit enable pattern for channels */

    // Micro register opcodes: ADJUST
     OPCODE_SET_GLOB_OFFSET_V1190A			= 0x5000,		/* Set global offset */
     OPCODE_READ_GLOB_OFFSET_V1190A			= 0x5100,		/* Read global offset */
     OPCODE_SET_ADJUST_CH_V1190A			= 0x5200,		/* Set channel nn adjust */
     OPCODE_READ_ADJUST_CH_V1190A			= 0x5300,		/* Read channel nn adjust */
     OPCODE_SET_RC_ADJ_V1190A				= 0x5400,		/* Set RC adjust of tdc 0n */
     OPCODE_READ_RC_ADJ_V1190A				= 0x5500,		/* Read RC adjust of tdc 0n */
     OPCODE_SAVE_RC_ADJ_V1190A				= 0x5600,		/* Save RC adjust on EEPROM */

    // Micro register opcodes: MISCELLANEOUS
     OPCODE_READ_TDC_ID_V1190A				= 0x6000,		/* Read programmed ID of TDC 0n */
     OPCODE_READ_MICRO_REV_V1190A			= 0x6100,		/* Read firmware revision of microcontroller */
     OPCODE_RESET_DLL_PLL_V1190A			= 0x6200,		/* Reset DLL and PLL */

    // Micro register opcodes: ADVANCED
     OPCODE_WRITE_SETUP_REG_V1190A			= 0x7000,		/* Write word nn into the scan path setup */
     OPCODE_READ_SETUP_REG_V1190A			= 0x7100,		/* Read word nn into the scan path setup */
     OPCODE_UPDATE_SETUP_REG_V1190A			= 0x7200,		/* Load the scan path setup */
     OPCODE_DEFAULT_SETUP_REG_V1190A		= 0x7300,		/* Reload the default scan path setup */
     OPCODE_READ_ERROR_STATUS_V1190A		= 0x7400,		/* Read errors in the TDC 0n status */
     OPCODE_READ_DLL_LOCK_V1190A			= 0x7500,		/* Read the DLL LOCK bit of the TDC 0n */
     OPCODE_READ_STATUS_STREAM_V1190A		= 0x7600,		/* Read the TDC 0n status */
     OPCODE_UPDATE_SETUP_TDC_V1190A			= 0x7700,		/* Load the scan path setup on TDC nn */

    // Micro register opcodes: DEBUG AND TEST
     OPCODE_WRITE_EEPROM_V1190A				= 0xC000,		/* Write 1 byte into the EEPROM */
     OPCODE_READ_EEPROM_V1190A				= 0xC100,		/* Read 1 byte from the EEPROM */
     OPCODE_MICROCONTROLLER_FW_V1190A		= 0xC200,		/* Read the controller firmware revision/date */
     OPCODE_WRITE_SPARE_V1190A				= 0xC300,		/* Write a 16 bit spare variable */
     OPCODE_READ_SPARE_V1190A				= 0xC400,		/* Read a 16 bit spare variable */
     OPCODE_EN_TEST_MODE_V1190A				= 0xC500,		/* Enable TDC test mode */
     OPCODE_DIS_TEST_MODE_V1190A			= 0xC600,		/* Disable TDC test mode */
     OPCODE_SET_TDC_TEST_OUTPUT_V1190A		= 0xC700,		/* Set TDC 0n signal test output */
     OPCODE_SET_DLL_CLOCK_V1190A			= 0xC800,		/* Set DLL clock source */
     OPCODE_READ_TDC_SETUP_SCAN_PATH_V1190A	= 0xC900,		/* Read all Setup Scan Path on TDC 0n */
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

typedef enum _ReadMode {
    LOW_RATE                                  = 0,
    HIGH_RATE                                 = 1
} ReadMode;

typedef enum _SetMode {
    ENABLE                                    = 1,
    DISABLE                                   = 0
} SetMode;

// ****************************************************************************************************

/*** Event storage ***/

typedef struct
{
    Data32                                    event_number;
    Data8                                     channel[MAXDATA];
    Data32                                    word_counter_tdc;
    Data32                                    tdc_data[MAXDATA];
    Data8                                     slope[MAXDATA];
} ntdc_event;

typedef struct
{
    int                                       n_words;
    Data32                                    tdc_data[MAXDATA];
} dataFIFO;

// ****************************************************************************************************

/*** CLASS ***/

class v1190a
{
 private :
    long                                    Handle;
    Data32                                  Address;
    CVDataWidth                             DataWidth;
    CVAddressModifier                       AddressModifier;
 public:

    v1190a(long handle, IniFile *inifile);
    ~v1190a();
    Data16 write_op_reg(Data32 address, int code);
    Data16 read_op_reg(Data32 address);
    void Reset();
    void Clear();
    void TestWR(Data16 value);
    void CheckStatus();
    void CheckCommunication();
    void SetTDCTestMode(Data16 mode);
    void SetTrigMatching();
    void SetTrigTimeSubstraction(Data16 mode);
    void SetTrigWindowWidth(long  windowWidth);
    void SetTrigWindowOffset(long windowOffset);
    void SetTrigSearchMargin(long searchMargin);
    void SetTrigRejectionMargin(long rejectMargin);
    void SetTrigConfiguration(IniFile *inifile);
    void GetTrigConfiguration();
    void SetTDCDetectionMode(Data16 mode);
    void SetTDCResolution(Data16 lsb);
    void SetTDCDeadTime(Data16 time);
    void SetTDCHeadTrailer(Data16 mode);
    void SetTDCEventSize(Data16 size);
    void SwitchChannels(IniFile *inifile);
    void SetIRQ(Data32 level, Data32 count);
    void Set(IniFile *inifile);
    Uint Read(v1718 *vme, string outputfilename);
};


#endif

