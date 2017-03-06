/* ISD5116 API */


#ifndef _ISD5116_H    /* Guard against multiple inclusion */
#define ISD5116_H

// Include required headers
#include <plib.h>
#include <GenericTypeDefs.h>
#include "settings.h"
#include <stdlib.h>

/* define Macros*/
/*============================================================================*/
/* Pin assignment definitions */
#define NOT_INT  PIN_B5
#define RAC      PIN_B4

/* I2C definitions */
#define ISD_DEVICE_ADDRESS 0x80

/* Command definitions */

#define COMMAND_POWER_UP    0x80
#define COMMAND_POWER_DOWN  0x00

#define DAB 0x40

#define FUNCTION_STOP    (0x0 << 3)
#define FUNCTION_PLAY    (0x5 << 3)
#define FUNCTION_RECORD  (0x2 << 3)
#define FUNCTION_MC      (0x7 << 3)
#define FUNCTION_READ    (0xc << 3)
#define FUNCTION_WRITE   (0x9 << 3)
#define FUNCTION_ERASE   (0xa << 3)

#define REGISTER_NO_ACTION     0x0
#define REGISTER_LOAD_ADDRESS  0x1
#define REGISTER_LOAD_CFG0     0x2
#define REGISTER_LOAD_CFG1     0x3

/* Status register definitions */

#define STATUS_PRB    0x08
#define STATUS_PD     0x10
#define STATUS_READY  0x20 
#define STATUS_OVF    0x40
#define STATUS_EOM    0x80

/* Configuration register definitions */

#define CFG0_VLPD  (1 << 0)

#define CFG0_OPA   1

#define CFG0_OPA_PD         0
#define CFG0_OPA_150OHM_SP  (0x1 << CFG0_OPA)
#define CFG0_OPA_8OHM_SP    (0x2 << CFG0_OPA)
#define CFG0_OPA_AUXOUT     (0x3 << CFG0_OPA)

#define CFG0_OPS   3

#define CFG0_OPS_VOL    0
#define CFG0_OPS_ANAIN  (0x1 << CFG0_OPS)
#define CFG0_OPS_FILTO  (0x2 << CFG0_OPS)
#define CFG0_OPS_SUM2   (0x3 << CFG0_OPS)

#define CFG0_AOPD  (1 << 5)

#define CFG0_AOS   6

#define CFG0_AOS_FTHRU  0
#define CFG0_AOS_INP    (0x1 << CFG0_AOS)
#define CFG0_AOS_VOL    (0x2 << CFG0_AOS)
#define CFG0_AOS_FILTO  (0x3 << CFG0_AOS)
#define CFG0_AOS_SUM1   (0x4 << CFG0_AOS)
#define CFG0_AOS_SUM2   (0x5 << CFG0_AOS)

#define CFG0_INSO  9

#define CFG0_INSO_AGC    0
#define CFG0_INSO_AUXIN  (1 << CFG0_INSO)

#define CFG0_AXPD  (1 << 10)

#define CFG0_AXG   11

#define CFG0_AXG_0  0
#define CFG0_AXG_3  (0x1 << CFG0_AXG)
#define CFG0_AXG_6  (0x2 << CFG0_AXG)
#define CFG0_AXG_9  (0x3 << CFG0_AXG)

#define CFG0_AIPD  (1 << 13)

#define CFG0_AIG   14

#define CFG0_AIG_6   0
#define CFG0_AIG_9   (0x1 << CFG0_AIG)
#define CFG0_AIG_12  (0x2 << CFG0_AIG)
#define CFG0_AIG_15  (0x3 << CFG0_AIG)


#define CFG1_AGPD  (1 << 0)

#define CFG1_FLPD  (1 << 1)

#define CFG1_FLD   2

#define CFG1_FLD_8KHZ   0
#define CFG1_FLD_64KHZ  (0x1 << CFG1_FLD)
#define CFG1_FLD_53KHZ  (0x2 << CFG1_FLD)
#define CFG1_FLD_4KHZ   (0x3 << CFG1_FLD)

#define CFG1_FLSO  4

#define CFG1_FLSO_SUM1   0
#define CFG1_FLSO_ARRAY  (1 << CFG1_FLSO)

#define CFG1_S2M   5

#define CFG1_S2M_BOTH   0
#define CFG1_S2M_ANAIN  (0x1 << CFG1_S2M)
#define CFG1_S2M_FILTO  (0x2 << CFG1_S2M)
#define CFG1_S2M_PD     (0x3 << CFG1_S2M)

#define CFG1_S1M   7

#define CFG1_S1M_BOTH   0
#define CFG1_S1M_SUM1   (0x1 << CFG1_S1M)
#define CFG1_S1M_INP    (0x2 << CFG1_S1M)
#define CFG1_S1M_PD     (0x3 << CFG1_S1M)

#define CFG1_S1S   9

#define CFG1_S1S_ANAIN  0
#define CFG1_S1S_ARRAY  (0x1 << CFG1_S1S)
#define CFG1_S1S_FILTO  (0x2 << CFG1_S1S)

#define CFG1_VOL   11

#define CFG1_VOL_0   0
#define CFG1_VOL_4   (0x1 << CFG1_VOL)
#define CFG1_VOL_8   (0x2 << CFG1_VOL)
#define CFG1_VOL_12  (0x3 << CFG1_VOL)
#define CFG1_VOL_16  (0x4 << CFG1_VOL)
#define CFG1_VOL_20  (0x5 << CFG1_VOL)
#define CFG1_VOL_24  (0x6 << CFG1_VOL)
#define CFG1_VOL_28  (0x7 << CFG1_VOL)

#define CFG1_VLS   14

#define CFG1_VLS_ANAIN  0
#define CFG1_VLS_SUM2   (0x1 << CFG1_VLS)
#define CFG1_VLS_SUM1   (0x2 << CFG1_VLS)
#define CFG1_VLS_INP    (0x3 << CFG1_VLS)

/* Delay definitions */

#define TRAC_LO 8
#define TSTOP   32
/*============================================================================*/
extern UINT8 status;
/* This function initializes the device. */
void isd_init(void);
/* Issue an I2C read status command. Return the status reported by the ISD. */
UINT8 isd_read_status(void);
/* Issue an I2C read status command. Return the address reported by the ISD. */
UINT16 isd_read_address(void);
/* Issue an I2C load command. */
void isd_load_command(UINT8 cmd, isd_i2c_stop_mode stop_mode);
/* Issue an I2C load command with address. */
void isd_load_command_address(UINT8 cmd, UINT16 addr, isd_i2c_stop_mode stop_mode) ;
/* Power up the ISD. */
void isd_power_up(void);
/* Power down the ISD. */
void isd_power_down(void);
/* Load CFG0 and CFG1 regs. Store to isd_cfg0_current and isd_cfg1_current. */
void isd_load_config(UINT16 isd_cfg0, UINT16 isd_cfg1);
/* This function initializes the configuration registers used for ISD5116 according to 
   values specified in isd_current_mode, isd_input, isd_gain, isd_volume global
   variables. */
void isd_set_config(void);
/* Record a message at the specified address on the ISD chip. */
void isd_record_address(UINT16 addr);
/* Play a message at specified address on the ISD chip. */
void isd_play_address(UINT16 addr);
/* Stop current operation. */
void isd_stop(void); 

#endif 

/* *****************************************************************************
 End of File
 */
