#include <plib.h>
#include <GenericTypeDefs.h>
#include <stdlib.h>

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

/* Type definitions */
typedef enum { ISD_NO_I2C_STOP, ISD_I2C_STOP } isd_i2c_stop_mode;

enum isd_input { isd_input_mic, isd_input_anain };

/* Variable definitions */
UINT8 current_isd_device_address = ISD_DEVICE_ADDRESS;

UINT16 isd_cfg0_current = 0; /* Value of CFG0 register currently loaded on the ISD */

UINT16 isd_cfg1_current = 0; /* Value of CFG0 register currently loaded on the ISD */

UINT8 isd_volume = 0; /* Volume attenuation */

UINT8 isd_input_gain = 0; /* Input gain. Only relevant if input is ANA IN. */

typedef enum { playing, recording, stopped } isd_mode;
isd_mode current_isd_mode = stopped; /* global variable for state of ISD chip */

/* ISD5116 API */

/* This function initializes the device. */
void
isd_init(void) {
  /* Make sure bit 3 and 4 of TRISC are set to 0 */
  /* Make sure to set RAC and INT pins (in appropriate TRIS register) are set as inputs (value 1) */

  /* Set up the I2C pins */
  //output_float(PIN_C3);
  //output_float(PIN_C4);
} /* isd_init */

/* Issue an I2C read status command. Return the status reported by the ISD. */
UINT8
isd_read_status(void) {
  UINT8 status, hi_addr, lo_addr;

  StartI2C1();
  MasterWriteI2C1(current_isd_device_address | 1); // Lowest bit = 1 => READ
  status = MasterReadI2C1();
  hi_addr = MasterReadI2C1();
  lo_addr = MasterReadI2C1();
  StopI2C1();

  return status;
} /* isd_read_status */

/* Issue an I2C read status command. Return the address reported by the ISD. */
UINT16
isd_read_address(void) {
  UINT8 status, hi_addr, lo_addr;
  UINT16 addr;

  StartI2C1();
  MasterWriteI2C1(current_isd_device_address | 1); // Lowest bit = 1 => READ
  status = MasterReadI2C1();
  hi_addr = MasterReadI2C1();
  lo_addr = MasterReadI2C1();
  StopI2C1();

  addr = hi_addr;

  addr <<= 8;

  addr |= lo_addr;

  return addr;
} /* isd_read_address */

/* Issue an I2C load command. */
void
isd_load_command(UINT8 cmd, isd_i2c_stop_mode stop_mode) {
  StartI2C1();
  MasterWriteI2C1(current_isd_device_address | 0); // Lowest bit = 0 => WRITE
  MasterWriteI2C1(cmd);
  if (stop_mode == ISD_I2C_STOP)
    StopI2C1();
} /* isd_load_command */

/* Issue an I2C load command with address. */
void
isd_load_command_address(UINT8 cmd, UINT16 addr, isd_i2c_stop_mode stop_mode) {
  StartI2C1();
  MasterWriteI2C1(current_isd_device_address | 0); // Lowest bit = 0 => WRITE
  MasterWriteI2C1(cmd);
  MasterWriteI2C1(addr >> 8);
  MasterWriteI2C1(addr & 0xff);
  if (stop_mode == ISD_I2C_STOP)
    StopI2C1();
} /* isd_load_command_address */

/* Power up the ISD. */
void
isd_power_up(void) {
  isd_load_command(COMMAND_POWER_UP, ISD_I2C_STOP);
} /* isd_power_up */

/* Power down the ISD. */
void
isd_power_down(void) {
  isd_load_command(COMMAND_POWER_DOWN, ISD_I2C_STOP);
  current_isd_mode = stopped;
} /* isd_power_down */

/* Load CFG0 and CFG1 registers. Store loaded values to isd_cfg0_current and 
   isd_cfg1_current. */
void
isd_load_config(UINT16 isd_cfg0, UINT16 isd_cfg1) {
  isd_load_command_address(COMMAND_POWER_UP | REGISTER_LOAD_CFG0, isd_cfg0, ISD_I2C_STOP);

  isd_load_command_address(COMMAND_POWER_UP | REGISTER_LOAD_CFG1, isd_cfg1, ISD_I2C_STOP);

  isd_cfg0_current = isd_cfg0;
  isd_cfg1_current = isd_cfg1;
} /* isd_load_config */
/* This function initializes the configuration registers used for ISD5116 according to 
   values specified in isd_current_mode, isd_input, isd_gain, isd_volume global
   variables. */
void
isd_set_config(void) {
  UINT16 isd_cfg0; /* Value of CFG0 register to instantiate on ISD chip */
  UINT16 isd_cfg1; /* Value of CFG1 register to instantiate on ISD chip */

  UINT8 ops; /* Indicates output source of Output MUX */
  UINT8 opa; /* Indicates output of Output MUX. */
  UINT16 aig, vol;

  if (current_isd_mode == playing) {
    opa = CFG0_OPA_8OHM_SP;
    ops = CFG0_OPS_VOL;
  }
  else {
    opa = CFG0_OPA_PD;
    ops = CFG0_OPS_FILTO;
  }

  isd_cfg0 = ops | /* Output MUX source */
    opa | /* Output MUX output */
    CFG0_INSO_AGC | /* AGC Amp input source */
    CFG0_AOPD | /* Power down ANA OUT */
    CFG0_AXPD; /* Power down AUX IN */
 
  isd_cfg1 = CFG1_FLD_8KHZ; /* Sampling rate */

  if (current_isd_mode == playing) {
    /* Output content of memory array */
    isd_cfg0 |=
      CFG0_AIPD; /* Power down ANA IN */

    vol = isd_volume;
    vol <<= CFG1_VOL;
    isd_cfg1 |= CFG1_AGPD |
      CFG1_FLSO_ARRAY |
      CFG1_S1M_PD |
      CFG1_S2M_FILTO |
      CFG1_VLS_SUM2 |
      vol;
  }
  else {
    /* Set for recording or feedthrough */
    if (ops != CFG0_OPS_VOL) {
      isd_cfg0 |= CFG0_VLPD; /* Power down volume control */
    }

    isd_cfg1 |= CFG1_FLSO_SUM1;

    // for microphone
    isd_cfg0 |= CFG0_AIPD; /* Power down ANA IN */

    isd_cfg1 |=
      CFG1_S1M_INP | /* Source of SUM1 Summing Amp is INP */
      CFG1_S2M_FILTO;
  }

  if (isd_cfg0_current != isd_cfg0 || isd_cfg1_current != isd_cfg1)
    isd_load_config(isd_cfg0, isd_cfg1);
} /* isd_set_config */

/* Record a message at current address on the ISD chip. */
void
isd_record(void) {
  current_isd_mode = recording;

  isd_set_config();

  isd_load_command(COMMAND_POWER_UP | FUNCTION_RECORD, ISD_I2C_STOP);
} /* isd_record */

/* Record a message at the specified address on the ISD chip. */
void
isd_record_address(UINT16 addr) {
  current_isd_mode = recording;

  isd_set_config();

  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_RECORD | REGISTER_LOAD_ADDRESS,
                           addr, ISD_I2C_STOP);
} /* isd_record_address */

/* Play a message at current address on the ISD chip. */
void
isd_play(void) {
  current_isd_mode = playing;

  /* Ensure ISD is set to output content of array */
  isd_set_config();

  isd_load_command(COMMAND_POWER_UP | FUNCTION_PLAY, ISD_I2C_STOP);
} /* isd_play */

/* Play a message at specified address on the ISD chip. */
void
isd_play_address(UINT16 addr) {
  current_isd_mode = playing;

  /* Ensure ISD is set to output content of array */
  isd_set_config();

  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_PLAY | REGISTER_LOAD_ADDRESS,
                           addr, ISD_I2C_STOP);
} /* isd_play_address */

/* Stop current operation. */
void
isd_stop(void) {
  isd_load_command(COMMAND_POWER_UP | FUNCTION_STOP, ISD_I2C_STOP);
  current_isd_mode = stopped;
} /* isd_stop */

/* Perform message cue on the ISD chip. */
void
isd_message_cue(void) {
  isd_load_command(COMMAND_POWER_UP | FUNCTION_MC, ISD_I2C_STOP);
} /* isd_message_cue */

/* Perform message cue from specified address on the ISD chip. */
void
isd_message_cue_address(UINT16 addr) {
  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_MC | REGISTER_LOAD_ADDRESS, addr,
                           ISD_I2C_STOP);
} /* isd_message_cue_address */

/* Enter digital mode. */
void
isd_enter_digital_mode(void) {
  isd_load_command(COMMAND_POWER_UP | DAB | FUNCTION_STOP, ISD_I2C_STOP);
} /* isd_enter_digital_mode */

/* Exit digital mode. */
void
isd_exit_digital_mode(void) {
  isd_load_command(COMMAND_POWER_DOWN | DAB | FUNCTION_STOP, ISD_I2C_STOP);
} /* isd_exit_digital_mode */

/*

/* Wait for RAC to go low, then go high. */
//void
//isd_wait_for_rac_pulse(void) {
//  /* Wait for RAC to go low */
//  for ( ; ; ) {
//    if (!input(RAC))
//      break;
//    delay_us(10);
//  }
//
//  /* Wait for RAC to go high */
//  for ( ; ; ) {
//    if (input(RAC))
//      break;
//    delay_us(10);
//  }
//} /* isd_wait_for_rac_pulse */

/* Load digital erase command at current address on ISD chip. */
/*

void
isd_digital_erase(void) {
  isd_enter_digital_mode();

  isd_load_command(COMMAND_POWER_UP | FUNCTION_ERASE, ISD_I2C_STOP);

  isd_stop();

  isd_wait_for_rac_pulse();

  isd_exit_digital_mode();
} */ /* isd_digital_erase */

/* Load digital erase command at specified address on ISD chip. Note that lower five
   address bits are ignored since address should be a row number. */

/*
void
isd_digital_erase_address(UINT16 addr) {
  isd_enter_digital_mode();

  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_ERASE | REGISTER_LOAD_ADDRESS, addr,
                           ISD_I2C_STOP);

  isd_stop();

  isd_wait_for_rac_pulse();

  isd_exit_digital_mode();
} */ /* isd_digital_erase_address */

//#byte SSPCON = 0x14
#define SSPEN 0x20

/* Perform a digital read at current address on ISD chip. */

void
isd_digital_read(UINT8 *buf, UINT16 len) {
  UINT16 i;

  if (len > 256)
    return;

  isd_enter_digital_mode();

  isd_load_command(COMMAND_POWER_UP | FUNCTION_READ, ISD_NO_I2C_STOP);

  /* Toggle SSPEN bit */
//  SSPCON &= ~SSPEN;
//  delay_ms(1);
//  SSPCON |= SSPEN;

  StartI2C1();

  MasterWriteI2C1(current_isd_device_address | 1); // Lowest bit = 1 => READ

  for (i = 0; i < len; i++)
    buf[i] = MasterReadI2C1();

  StopI2C1();

  isd_exit_digital_mode();
}  /* isd_digital_read */

/* Perform a digital read at specified address on ISD chip. */
void
isd_digital_read_address(UINT16 addr, UINT8 *buf, UINT16 len) {
  UINT16 i;

  if (len > 256)
    return;

  isd_enter_digital_mode();

  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_READ | REGISTER_LOAD_ADDRESS,
                           addr, ISD_NO_I2C_STOP);

  /* Toggle SSPEN bit */
  //SSPCON &= ~SSPEN;
  //SSPCON |= SSPEN;

  StartI2C1();

  MasterWriteI2C1(current_isd_device_address | 1); // Lowest bit = 1 => READ

  for (i = 0; i < len; i++) {
    buf[i] = MasterReadI2C1();
  }

  StopI2C1();

  isd_exit_digital_mode();
} /* isd_digital_read_address *

/* Wait for device to report it is ready */
void
isd_wait_for_status_ready(void) {
  UINT8 status;

  for ( ; ; ) {
    status = isd_read_status();

    if (status & STATUS_READY)
      break;

    //delay_us(10);
  }
} /* isd_wait_for_status_ready */

/* Perform a digital write at current address on ISD chip. */
void
isd_digital_write(UINT8 *buf, UINT16 len) {
  UINT16 i;

  if (len > 256)
    return;

  isd_enter_digital_mode();

  isd_load_command(COMMAND_POWER_UP | FUNCTION_WRITE, ISD_NO_I2C_STOP);

  for (i = 0; i < len; i++)
    MasterWriteI2C1(buf[i]);

  StopI2C1();

  isd_wait_for_status_ready();

  isd_exit_digital_mode();
} /* isd_digital_write */

/* Perform a digital write at specified address on ISD chip. */
void
isd_digital_write_address(UINT16 addr, UINT8 *buf, UINT16 len) {
  UINT16 i;

  if (len > 256)
    return;

  isd_enter_digital_mode();

  isd_load_command_address(COMMAND_POWER_UP | FUNCTION_WRITE | REGISTER_LOAD_ADDRESS,
                           addr, ISD_NO_I2C_STOP);

  for (i = 0; i < len; i++)
    MasterWriteI2C1(buf[i]);

  StopI2C1();

  isd_wait_for_status_ready();

  isd_exit_digital_mode();
} /* isd_digital_write_address */



