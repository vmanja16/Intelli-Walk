#include "intelli-walk.h"


UINT32 test_array[10], test_array2[10];
volatile int index = 0;
int L = 1;
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
    mPORTASetPinsDigitalOut(0xff);              // set LEDS as digital outputs
	mPORTAClearBits(0xff);                      // clear LEDS
    	
    AD1PCFG = 0xFFFF;                           // ALL digital
    TRISB = 0x03;                               // RB 0,1 as Input PIN (ECHO 1,2)                                // RB4 is Output PIN (TRIGGER)  
    TRISD = 0x80;        // LCD Pins as Output
  
    TRISDbits.TRISD10 = 0b1; // Pushbutton is input
    TRISDbits.TRISD11 = 0b1; // example encoder input
    
}
void Initializations(){
    /*=========     GPIO      ===========*/
    gpio_init();
    /*=========     TIMER1    ===========*/
    ultrasonic_init();
    /*=========     TIMER4   (PUSHBUTTON) ===========*/
    timer4_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    //timer5_init();
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    //pwm1_init(); // Output is RD0
    /*=========     I2C       ===========*/
    isd_init();
    /*=========== Interrupts    ========*/
    INTEnableSystemMultiVectoredInt();
}
void ShortDelay(UINT32 DelayCount){                   // Delay Time (CoreTimer Ticks)  
  UINT32 StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime 
  while ((UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {} ;
} 
void delay_seconds(UINT32 secs){
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
        if(index>=10){
            index = 0;
            count = 1;
            if(mode == PLAYBACK_PATH){
                asm("nop");
            }
            mode = RECORDING_VOICE;
            
            asm("nop");
            return;
        }
        if(mode==RECORDING_PATH)test_array[index] = encoder_count;
        if(mode==PLAYBACK_PATH)test_array2[index] = encoder_count;
        index++;
        encoder_count=0;
    }    
    
    mT5ClearIntFlag(); // clear the interrupt flag
    asm("nop");
 }


/* 0x3F on PWM = 2A motor encoders per half second*/
int main(void)
{       
    unsigned char ob_1 = 0, ob_2 = 0;
    int i = 0;
    int  oc_values[10] = {0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F};    
    //UINT32 * ptr = NULL;
   
    Initializations();
    
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
            OC1RS = test_array[index] * ENCODER_MULTIPLIER;
            
        }
        // kill motor if not testing   
        else{i = 0; OC1RS=0;}
    }
    
    */
    
    /*    TESTING ISD5116 LOOP*/
  //  /*
    while(1)
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
      
          //  if(isd_ready()) isd_record_address(0x100 << 5);
           // ShortDelay(7000000* US_TO_CT_TICKS);
            
      //       PLAY BEEPS
            if (Obstacle_1){
                if(isd_ready())
                    isd_play_address(0x0000);
                ShortDelay(1000000 * US_TO_CT_TICKS);
                isd_stop();
                status = isd_read_status();
                asm("nop");
            }
            else if (Obstacle_2){
                if(isd_ready())
                    //0x028
                    isd_play_address(0x100 << 5);
                ShortDelay(6000000 * US_TO_CT_TICKS);
                isd_stop();
            }
            status = isd_read_status();
            address = isd_read_address();
            isd_power_down();
            asm("nop");
        }
    
    }
    //*/
    asm("nop");
}