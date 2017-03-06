#include "pushbutton.h"
void timer4_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = 0x0F42; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T4CONSET = 0x8020; // Start the timer
    // set up the core timer interrupt with a Priority of 3 and zero sub-priority
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_3);
}

void __ISR(_TIMER_4_VECTOR, ipl3) _Timer4Handler(void){
    if(PORTDbits.RD11){
        mPORTAClearBits(0xF0);
        // call mode_select
        // might need a static var to hold state of pushbuttons between ISRs
    }
        else{
            //mPORTAClearBits(0xF0);
        }
    mT4ClearIntFlag(); // clear the interrupt flag
    
 }
