#include "pushbutton.h"

void pushbutton_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = SAMPLING_PERIOD; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T4CONSET = 0x8020; // Start the timer
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_3);// interrupt with a Priority of 3 and zero sub-priority
}

void __ISR(_TIMER_4_VECTOR, ipl3) _Timer4Handler(void){

     // static vars keep their value between interrupts (0 = pushed)
    static unsigned char pb1_status = 1, pb2_status = 1, pb3_status = 1;
    static op_mode next_mode = IDLE;
    static next_path = 0;
    int i = 0;
    
    // PB_2: Choose a mode from IDLE
    
    if((pb2_status == 0) && PORTBbits.RB11){
        
        
        if (mode == IDLE){
            next_mode = PLAYBACK_PATH;
            next_path = (next_path+1) % 3;
            for(i=0; i< (next_path+1); i++){
                single_beep();ShortDelay(10000*US_TO_CT_TICKS);
            }

            //PORTAbits.RA2 = !PORTAbits.RA2;
            LATAbits.LATA2 = !LATAbits.LATA2;
        }    
    }
    
    if((pb3_status==0) && PORTBbits.RB12){
        if(mode==IDLE){
            path = next_path;
            mode = next_mode;
            LATAbits.LATA2 = 1;
        }
    }
    
    // PB2 pushed
    /*
    if((pb2_status != PUSHBUTTON_2) && PUSHBUTTON_2){   
        if(mode==IDLE){
            path = next_path;
            mode = next_mode;
            
        }
    }
    */
    // update button status
    pb1_status = PORTBbits.RB10;
    pb2_status = PORTBbits.RB11;
    pb3_status = PORTBbits.RB12;
    
    mT4ClearIntFlag(); // clear the interrupt flag
 }
