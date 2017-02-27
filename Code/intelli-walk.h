
#ifndef _INTELLI-WALK_H    /* Guard against multiple inclusion */
#define _INTELLI-WALK_H

/* INCLUDED HEADERS*/
#include <plib.h>        /* PIC32 peripheral library */
#include <stdio.h>       /* Standard Input-Outut     */
#include <xc.h>          /* XC32 Compiler Library    */
#include "ultrasonic.h"  /* Ultrasonic functions     */
#include "isd5116.h"     /* ISD5116 I2C functions    */

/* DEFINITIONS*/

//  System Clock = 80 MHz,  Peripheral Bus = 40 MHz
//  Primary Osc w/PLL (XT+,HS+,EC+PLL)
//  Input Divider    2x Divider
//  Multiplier      20x Multiplier
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF
#define	SYS_FREQ	              80000000		 // frequency we're running at
#define CPU_CLOCK_HZ             (80000000UL)    // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)    // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)    // Peripheral Bus  in Hz 
#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)    // uS to CoreTimer Ticks 

/* GLOBAL VARIABLES*/
// volatile --> variables used in main && ISR
unsigned int Timer1_count = 0; // Number of Timer 1 Interrupts 
unsigned int Echo1_count = 0;  // Number of Timer1 interrupts in echo1
unsigned int Echo2_count = 0;  // Number of Timer1 interrupts in echo2
volatile unsigned char Obstacle_1 = 0; // Boolean signifying Ultra1 found an obstacle
volatile unsigned char Obstacle_2 = 0;


#endif /* _INTELLI-WALK_H */
