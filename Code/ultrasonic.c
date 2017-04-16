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
    if (Timer1_count <= 1){ PORTBbits.RB6 =  1; PORTBbits.RB8 = 1;}
    else{PORTBbits.RB6 = 0; PORTBbits.RB8 = 0;} 
    
    // Updating Echo reading
    if (PORTBbits.RB7){Echo1_count ++;}
    else{
        if (Echo1_count > 5){
            if (Echo1_count < ULTRA_THRESHOLD_1 ){
                Obstacle_1 = 1; // Obstacle detected
            }
            else{
                Obstacle_1 = 0; // No obstacle
            }
            Echo1_count = 0; // reset Echo count
        }
    }
    if (PORTBbits.RB9){Echo2_count ++;}
    else{
        if (Echo2_count > 5){
            if (Echo2_count < ULTRA_THRESHOLD_2 ){
                Obstacle_2 = 1; // Obstacle detected
            }
            else{
                
                Obstacle_2 = 0; // No obstacle
            }
            Echo2_count = 0; // reset Echo count
        }
    }
    mT1ClearIntFlag(); // clear the interrupt flag
    
 }
