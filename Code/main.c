#include "intelli-walk.h"

/*
//========================= FOR TESTING main solo! =============================
#include <plib.h>
#define	SYS_FREQ	              80000000		   // frequency we're running at
#define CPU_CLOCK_HZ             (80000000UL)      // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)      // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)      // Peripheral Bus  in Hz 
#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)      // uS to CoreTimer Ticks 
#define I2C_BRG 0x0c2 // I2C BRG value for 100 kHz
//============================================================================
*/

UINT32 test_array[10], test_array2[10];
volatile int idx = 0;
volatile UINT32 count = 1;

void pwm1_init(){
    OC1CON = 0x0000; // PWM off
    T2CON  = 0x0000; // Timer 2 w/ prescaler of 64
    OC1R   = 0x0064; // Original duty cycle 
    OC1RS  = 0x0064; // OC1RS is used to load new duty cycles!
    OC1CON = 0x0006; // PWM Mode
    PR2    = 0x00AF; // Timer's period
    T2CONSET  = 0x8020; // start Timer2
    OC1CONSET = 0x8000; // start PWM
 }
void gpio_init(){	
    AD1PCFG = 0xFFFF;                           // ALL digital
    TRISB = 0x00;                               // RB 0,1 as Input PIN (ECHO 1,2)                                // RB4 is Output PIN (TRIGGER)  
    TRISD = 0x80;
  
    
    
    
    
    // Ultrasonic ECHO inputs
    TRISBbits.TRISB7 = 0b1;
    TRISBbits.TRISB9 = 0b1;
    
    // Pushbuttons
    TRISBbits.TRISB10 = 0b1; // Pushbutton is input
    TRISBbits.TRISB11 = 0b1; // Pushbutton is input
    TRISBbits.TRISB12 = 0b1; // pushbutton is input
    
    
    TRISDbits.TRISD11 = 0b1; // example encoder input
    
}
void Initializations(){
    /*=========     GPIO      ===========*/
    gpio_init();
    /*=========     TIMER1    ===========*/
    //ultrasonic_init();
    /*=========     TIMER4   (PUSHBUTTON) ===========*/
    //pushbutton_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    //timer5_init();
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    pwm1_init(); // Output is RD0
    /*=========     I2C       ===========*/
    //isd_init();
    /*=========== Interrupts    ========*/
    INTEnableSystemMultiVectoredInt();
}
void ShortDelay(UINT32 DelayCount){                   // Delay Time (CoreTimer Ticks)  
  UINT32 StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime 
  while ((UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {} ;
} 
void delay_seconds(UINT8 secs){
    ShortDelay(secs*1000000*US_TO_CT_TICKS);
}
void timer5_init(void){
    T5CON = 0x0000; // Stop the timer and clear the control register,
    TMR5 = 0x0000; // Clear the timer register
    PR5 = 0x0F42; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T5CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a Priority of 4 and zero sub-priority
    ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_2);
}
/* MOTOR ENCODER SAMPLER AND SUMMER*/
/*
void __ISR(_TIMER_5_VECTOR, ipl2) _Timer5Handler(void){
   
    // Grey Counter bit
    static UINT8 toggle = 0;
    
    //Not in a path  mode
    //if(mode==IDLE){toggle = 0; count = 1; mT5ClearIntFlag(); return;}
    if (!( (mode==RECORDING_PATH) || (mode==PLAYBACK_PATH) ) ){
        toggle = 0; count = 1; mT5ClearIntFlag(); return;
    }
    
    // Increment encoder_val if bit has been toggled!
    if (PORTDbits.RD11 != toggle){
        encoder_count++;
    }    
    // Update toggle value 
    toggle = PORTDbits.RD11;
    
    // Record count in memory
    count = (++count) % 5000;
    if(count==0){
        //we only have 10 spaces in array atm
        if(idx>=10){
            idx = 0;
            count = 1;
            if(mode == PLAYBACK_PATH){
                asm("nop");
            }
            mode = RECORDING_VOICE;
            
            asm("nop");
            return;
        }
        if(mode==RECORDING_PATH)test_array[idx] = encoder_count;
        if(mode==PLAYBACK_PATH)test_array2[idx] = encoder_count;
        idx++;
        encoder_count=0;
    }    
    
    mT5ClearIntFlag(); // clear the interrupt flag
    asm("nop");
 }
*/



void motor1_off(){
    PORTBbits.RB4 = 0;
    OC1RS = 0x00;
}
void motor1_on(UINT16 speed){
    PORTBbits.RB4 = 1;
    OC1RS = speed;
}


/* 0x5F on PWM = 2A motor encoders per half second*/
int main(void)
{       
    unsigned char ob_1 = 0, ob_2 = 0;
    int i = 0;
    
   int  oc_values[10] = {0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F, 0x9F, 0x1F, 0x2F, 0x00};    
   
    /* RUN INITIALIZATIONS*/
    Initializations();
    
    while(1){
        motor1_off();
        delay_seconds(3);
        motor1_on(0xFF);
        delay_seconds(1);
    }
    /* TSTING PUSHBUTTON LOOP*/
    /*
    PORTBbits.RB8 = 1;
    PORTBbits.RB9 = 0;
    while(1){
        
    }
    
    */
    /* TESTING ULTRASONIC LOOP*/
    /*
    while(1){
        ob_1 = Obstacle_1;
        ob_2 = Obstacle_2;
        if (ob_1 || ob_2){
            // do stuff
        }
    }
    */
    /* TESTING PWM + ENCODER LOOP*/
   /*
    for(i=0;i<10;i++){test_array[i]=0x00; test_array2[i] = 0x00;} // initialize test_array
    i=0;
    while(1){
        if (mode==RECORDING_PATH){
            PORTA = 0xF0;
            //delay_seconds();
            i = (++i) % 10;
            OC1RS = 0x7F;
        }
        else if (mode==PLAYBACK_PATH){
            PORTA = 0x0F;
            // get PWM from encoder array
            OC1RS = test_array[idx] * ENCODER_MULTIPLIER;
            
        }
        // kill motor if not testing   
        else{i = 0; OC1RS=0;}
    }
    
    */
    /*    TESTING ISD5116 LOOP*/
    /*    while(1)
    {
        
        //if(PORTDbits.RD11){mPORTASetBits(0xFF);}
        //else{mPORTAClearBits(0xFF);}
        
        ShortDelay(50000 * US_TO_CT_TICKS) ;
        ob_1 = Obstacle_1; ob_2 = Obstacle_2;
        if (!(Obstacle_1||Obstacle_2)){
            //i = (i + 1) % 10;
            //OC1RS = oc_values[i];            
        }        
        else {
            // Debug Record!
            isd_power_up();
            ShortDelay(100000*US_TO_CT_TICKS);
      
              // RECORD IF RECORDINGPATH MODE
            if(mode == RECORDING_PATH){
                if(isd_ready()) isd_record_address(0x100 << 5);
                ShortDelay(7000000* US_TO_CT_TICKS);
                mode = RECORDING_VOICE; 
                continue;
            }
      //       PLAY BEEPS
            if (Obstacle_1){
                if(isd_ready())
                    isd_play_address(0x100 << 5);
                ShortDelay(7000000 * US_TO_CT_TICKS);
                isd_stop();
                status = isd_read_status();
                asm("nop");
            }
            //
            else if (Obstacle_2){
                if(isd_ready())
                    //0x028
                    isd_play_address(0x100 << 5);
                ShortDelay(6000000 * US_TO_CT_TICKS);
                isd_stop();
            }
            ///
            status = isd_read_status();
            address = isd_read_address();
            isd_power_down();
            asm("nop");
        }
    
    } */
    
    asm("nop");
} // end main
