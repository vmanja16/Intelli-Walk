
#ifndef _INTELLI-WALK_H    /* Guard against multiple inclusion */
#define _INTELLI-WALK_H

/* INCLUDED HEADERS*/
#include <plib.h>        /* PIC32 peripheral library */
#include <stdio.h>       /* Standard Input-Outut     */
#include <xc.h>          /* XC32 Compiler Library    */
#include "settings.h"
#include "ultrasonic.h"  /* Ultrasonic functions     */
#include "isd5116.h"     /* ISD5116 I2C functions    */
#include "pushbutton.h"  /* PUSHBUTTONS ISR functions*/
#include "encoder.h"     /* MOTOR_ENCODER functions  */
/* PRAGMAS*/
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF


/* GLOBAL VARIABLES*/
// volatile --> variables used in main && ISR
volatile UINT8 encoder_count = 1;
unsigned int Timer1_count = 0; // Number of Timer 1 Interrupts 
unsigned int Echo1_count = 0;  // Number of Timer1 interrupts in echo1
unsigned int Echo2_count = 0;  // Number of Timer1 interrupts in echo2
volatile unsigned char Obstacle_1 = 0; // Boolean signifying Ultra1 found an obstacle
volatile unsigned char Obstacle_2 = 0;
volatile op_mode mode = IDLE;

UINT8 status = 0xFF;
UINT16 address = 0x1234;

#endif /* _INTELLI-WALK_H */
