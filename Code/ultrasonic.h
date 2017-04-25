#ifndef _ULTRASONIC_H    /* Guard against multiple inclusion */
#define _ULTRASONIC_H

#include <xc.h>
#include <plib.h>
#include "settings.h"
#include "ports.h"

#define MS_50 5000
#define ULTRA_THRESHOLD_1 200
#define ULTRA_THRESHOLD_2 60

// Timer 1 configuration function
void ultrasonic_init(void);

// Timer1 ISR globals
extern unsigned int Timer1_count; // Number of Timer 1 Interrupts 
extern unsigned int Echo1_count;  // Number of Timer1 interrupts in echo1
extern unsigned int Echo2_count;  // Number of Timer1 interrupts in echo2
extern volatile unsigned char Obstacle_1; // Boolean signifying Ultra1 found an obstacle
extern volatile unsigned char Obstacle_2;


#endif /* _ULTRASONIC_H */


