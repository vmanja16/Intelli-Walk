#include "pushbutton.h"

void timer4_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = SAMPLING_PERIOD; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T4CONSET = 0x8020; // Start the timer
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_3);// interrupt with a Priority of 3 and zero sub-priority
}

void __ISR(_TIMER_4_VECTOR, ipl3) _Timer4Handler(void){

     // static vars keep their value between interrupts
    static unsigned char pb_status = 1; // 0  = pushed (or not depending on circuit)

    // Toggle LEDs if status changed
    if((pb_status != PUSH_BUTTON_1) && PUSH_BUTTON_1){
        mPORTAToggleBits(0xFF);
        // change operational 'mode' variable
        if(mode==IDLE){mode=RECORDING_PATH;}
        else if (mode==RECORDING_VOICE){mode = PLAYBACK_PATH;}
        else if (mode==PLAYBACK_PATH){mode = IDLE;}
    }
    
    // update button status
    pb_status = PUSH_BUTTON_1;
    
    mT4ClearIntFlag(); // clear the interrupt flag
 }
