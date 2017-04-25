#ifndef _SETTINGS_H    /* Guard against multiple inclusion */
#define _SETTINGS_H
//  System Clock = 80 MHz,  Peripheral Bus = 40 MHz
//  Primary Osc w/PLL (XT+,HS+,EC+PLL)
//  Input Divider    2x Divider
//  Multiplier      20x Multiplier
#define	SYS_FREQ	              80000000		   // frequency we're running at
#define CPU_CLOCK_HZ             (80000000UL)      // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)      // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)      // Peripheral Bus  in Hz 
#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)      // uS to CoreTimer Ticks 
#define I2C_BRG 0x0c2 // I2C BRG value for 100 kHz

/* Type definitions */

  // PIC Operating modes
typedef enum {IDLE, RECORDING_PATH, RECORDING_VOICE, PLAYBACK_VOICE, PLAYBACK_PATH, MOTOR_PSSC} op_mode;
  // ISD Operating modes
typedef enum { playing, recording, stopped } isd_mode;
  // ISD Stop Modes
typedef enum { ISD_NO_I2C_STOP, ISD_I2C_STOP } isd_i2c_stop_mode;

typedef enum {OBSTACLE_DETECTED, POSSIBLE_OBSTACLE, POSSIBLE_CLEAR, FULL_CLEAR} ultrasonic_state;

void ShortDelay(UINT32 DelayCount);
void delay_seconds(UINT8 secs);
void single_beep();


#endif /* _SETTINGS_H */
