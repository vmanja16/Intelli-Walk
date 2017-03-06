#include "ultrasonic.h"

/* ULTRASONIC POLLING TIMER INITIALIZATION*/
void ultrasonic_init(void){
    T1CON = 0x0000; // Stop the timer and clear the control register,
    TMR1 = 0x0000; // Clear the timer register
    PR1 = 0x0190; // Load the period register with value 400, since 400 ticks = 10 microsecond
    T1CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a Priority of 2 and zero sub-priority
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2);
}

/* ULTRASONIC SENSORS ISR*/
void __ISR(_TIMER_1_VECTOR, ipl2) _Timer1Handler(void){
    
    Timer1_count = (Timer1_count+1)  % MS_50; // Update timer count mod 50 ms
    // 10 microsecond pulse to sensors!
    if (Timer1_count <= 1){ PORTBbits.RB4 =  1; PORTDbits.RD3 = 1;}
    else{PORTBbits.RB4 = 0; PORTDbits.RD3 = 0;} 
    
    // Updating Echo reading
    if (PORTBbits.RB0){Echo1_count ++;}
    else{
        if (Echo1_count > 5){
            if (Echo1_count < ULTRA_THRESHOLD_1 ){
                mPORTASetBits(0xFF);
                Obstacle_1 = 1; // Obstacle detected
                OC1RS = 0x0000; // shutoff the PWM
            }
            else{
                mPORTAClearBits(0xFF);
                Obstacle_1 = 0; // No obstacle
            }
            Echo1_count = 0; // reset Echo count
        }
    }
    if (PORTBbits.RB1){Echo2_count ++;}
    else{
        if (Echo2_count > 5){
            if (Echo2_count < ULTRA_THRESHOLD_2 ){
                mPORTASetBits(0x0F);
                Obstacle_2 = 1; // Obstacle detected
                OC1RS = 0x0000; // shutoff the PWM
            }
            else{
                //mPORTAClearBits(0x0F);
                Obstacle_2 = 0; // No obstacle
            }
            Echo2_count = 0; // reset Echo count
        }
    }
    mT1ClearIntFlag(); // clear the interrupt flag
    
 }
