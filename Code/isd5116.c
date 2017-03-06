#include "isd5116.h"

/* Variable definitions */
UINT8 current_isd_device_address = ISD_DEVICE_ADDRESS;

UINT16 isd_cfg0_current = 0; /* Value of CFG0 register currently loaded on the ISD */

UINT16 isd_cfg1_current = 0; /* Value of CFG0 register currently loaded on the ISD */

UINT8 isd_volume = 0; /* Volume attenuation */

UINT8 isd_input_gain = 0; /* Input gain. Only relevant if input is ANA IN. */

isd_mode current_isd_mode = stopped; /* global variable for state of ISD chip */

/* I2C FUNCTIONS*/
unsigned char i2c_read(){
    IdleI2C1();
    return MasterReadI2C1();
}
unsigned int i2c_write(unsigned char data){
    IdleI2C1();
    return MasterWriteI2C1(data);
}
void i2c_stop(){
    IdleI2C1();
    StopI2C1();
}
void i2c_start(){
    IdleI2C1();
    StartI2C1();
}

/* ISD FUNCTIONS*/
void
isd_init(void) {
  /* Set up the I2C pins */
  // need to set baudrate for scl
   OpenI2C1(I2C_ON,I2C_BRG);  
   IdleI2C1();
   /*
   UINT32 StartTime;
   
   StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
   while ((UINT32)(ReadCoreTimer() - StartTime) < 5000000 ) {} ;
   //isd_record_address(0);
   StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
   while ((UINT32)(ReadCoreTimer() - StartTime) < (10000 * US_TO_CT_TICKS) ) {} ;
     
   while(1){
       StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
       while ((UINT32)(ReadCoreTimer() - StartTime) < (10000 * US_TO_CT_TICKS) ) {} ;
       
       isd_power_up();
       StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
       while ((UINT32)(ReadCoreTimer() - StartTime) < (10000 * US_TO_CT_TICKS) ) {} ;
       isd_play_address(0x000);
       status = isd_read_status();
       status = status;
        StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
        while ((UINT32)(ReadCoreTimer() - StartTime) < (1000000 * US_TO_CT_TICKS) ) {} ;
       isd_stop();
       StartTime= ReadCoreTimer();         // Get CoreTimer value for StartTime 
        while ((UINT32)(ReadCoreTimer() - StartTime) < (100000000 * US_TO_CT_TICKS) ) {} ;
       isd_power_down();
       status = isd_read_status();
       status = status;
       
     
   }*/
} /* isd_init */

UINT8 isd_ready(void){
    return (isd_read_status() &0x20) >> 5;
}


UINT8
isd_read_status(void) {
  UINT8 status, hi_addr, lo_addr;

  i2c_start();
  i2c_write(current_isd_device_address | 1); // Lowest bit = 1 => READ
  
  status = i2c_read();
  hi_addr = i2c_read();
  lo_addr = i2c_read();
  i2c_stop();

  return status;
} /* isd_read_status */

UINT16
isd_read_address(void) {
  UINT8 status, hi_addr, lo_addr;
  UINT16 addr;

  
  i2c_start();
  i2c_write(current_isd_device_address | 1); // Lowest bit = 1 => READ
  
  status = i2c_read();
  hi_addr = i2c_read();
  lo_addr = i2c_read();
  i2c_stop();

  addr = hi_addr;

  addr <<= 8;

  addr |= lo_addr;

  return addr;
} /* isd_read_address */

void
isd_load_command(UINT8 cmd, isd_i2c_stop_mode stop_mode) {
  
  i2c_start();
  
  i2c_write(current_isd_device_address | 0); // Lowest bit = 0 => WRITE
  
  i2c_write(cmd);
  
  if (stop_mode == ISD_I2C_STOP)
    i2c_stop();
} /* isd_load_command */

void
isd_load_command_address(UINT8 cmd, UINT16 addr, isd_i2c_stop_mode stop_mode) {
  
  i2c_start();
  
  i2c_write(current_isd_device_address | 0); // Lowest bit = 0 => WRITE
  
  i2c_write(cmd);
  
  i2c_write(addr >> 8);
  
  i2c_write(addr & 0xff);
  
  if (stop_mode == ISD_I2C_STOP)
    i2c_stop();
} /* isd_load_command_address */

void
isd_power_up(void) {
  isd_load_command(COMMAND_POWER_UP, ISD_I2C_STOP);
} /* isd_power_up */

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



