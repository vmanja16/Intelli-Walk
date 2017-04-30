#include "pushbutton.h"

void pushbutton_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = SAMPLING_PERIOD; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T4CONSET = 0x8020; // Start the timer
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_2);// interrupt with a Priority of 3 and zero sub-priority
}

void __ISR(_TIMER_4_VECTOR, ipl2) _Timer4Handler(void){

     // static vars keep their value between interrupts (0 = pushed)
    static unsigned char pb1_status = 1, pb2_status = 1, pb3_status = 1;
    static op_mode next_mode = IDLE;
    static int next_playback_path = 0;
    int i = 0;
    unsigned char b10 = PORTBbits.RB10;
    unsigned char b11 = PORTBbits.RB11;
    unsigned char b12 = PORTBbits.RB12;
    
    // ON PB_2 PRESS: Choose a mode from IDLE
    
    if((pb2_status == 0) && b11){
        
        LATAbits.LATA2 = !LATAbits.LATA2;
        if (mode == IDLE){
            
            // Update next_playback if exists
            next_playback_path = (next_playback_path+1) % 10;
            if(paths[next_playback_path][0]==0xFF){
                next_playback_path=playback_path;
            }
            // Change mode if path exists (and beep_number)
            if(paths[next_playback_path][0]!=0xFF){
                next_mode = PLAYBACK_PATH;
                for(i=0; i< (next_playback_path+1); i++){
                   single_beep();ShortDelay(5000*US_TO_CT_TICKS);
                }
            }
        }    
        
    }
    // ON PB3 PRESS (SELECT)
    else if((pb3_status==0) && b12){
        LATAbits.LATA2 = !LATAbits.LATA2;
        if(mode==IDLE){
       //     idx = 0;
            playback_path = next_playback_path;
            mode = next_mode;
        }
        else if(mode==RECORDING_PATH){
            mode = IDLE;
            record_path++; 
            single_beep_high();
            next_playback_path = 0;
            next_mode = IDLE;
        }
        else if(mode==PLAYBACK_PATH){
            mode = IDLE; quick_beep();
        }
        
    }
    // ON PB1 PRESS
    else if((pb1_status == 0) && b10){
                LATAbits.LATA2 = !LATAbits.LATA2;
          if(mode==IDLE){
              next_mode = RECORDING_PATH; single_beep();
          }
      }
    
    // update button status
    pb1_status = b10;
    pb2_status = b11;
    pb3_status = b12;
    
    mT4ClearIntFlag(); // clear the interrupt flag
 }
