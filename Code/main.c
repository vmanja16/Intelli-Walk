#include "intelli-walk.h"
/*
//========================= FOR TESTING main solo! =============================
#include <plib.h>
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF
#define	SYS_FREQ	              80000000		   // frequency we're running at
#define CPU_CLOCK_HZ             (80000000UL)      // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)      // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)      // Peripheral Bus  in Hz 
#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)      // uS to CoreTimer Ticks 
#define I2C_BRG 0x0c2 // I2C BRG value for 100 kHz
//============================================================================
*/
// EXTRA GLOBALS!
UINT16 test_array[100], test_array2[100];
volatile int idx = 0;
volatile UINT32 count = 1;
char X = 1;



int path_0[10] ={0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
int path_1[10] ={0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F};
int path_2[10] ={0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F};
int * paths [3] = {path_0, path_1, path_2};



/* MOTOR ENCODER SAMPLER AND SUMMER*/
void timer5_init(void){
    T5CON = 0x0000; // Stop the timer and clear the control register,
    TMR5 = 0x0000; // Clear the timer register
    PR5 = 0x0F42; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T5CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a Priority of 4 and zero sub-priority
    ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_2);
}
void __ISR(_TIMER_5_VECTOR, ipl2) _Timer5Handler(void){
   
    // Grey Counter bit
    static UINT8 toggle1 = 0, toggle2 = 0;
    unsigned char ob_2 = Obstacle_2;
    
    // Don't deal w/ encoder/timer if obstacle detected, OC taken care of in Main!
    if(ob_2){mT5ClearIntFlag();return;}
    // for playback, just deal w/ count and index
    if(mode==PLAYBACK_PATH){if( ( (++count) % 5000)==0){idx++;} mT5ClearIntFlag(); return;}
    // Non Record/Playback --> dont care!
    if (!(mode==RECORDING_PATH)){toggle1 = 0; count = 1; mT5ClearIntFlag(); return;}
    
    // Record count in memory while recording
    count = (++count) % 5000;
    
    //==============================ENCODER LOGIC========================//
    // Increment encoder_val if bit has been toggle1d!
    if (LEFT_ENCODER != toggle1){encoder1_count++;}
    if (RIGHT_ENCODER != toggle2){encoder2_count++;}
    // Update toggle1 value 
    toggle1 = LEFT_ENCODER;
    toggle2 = RIGHT_ENCODER;
    
    if(count==0){
        //we only have 10 spaces in array atm
        if(idx>=100){
            idx = 0;
            count = 1;
            mode = RECORDING_VOICE;
            
            asm("nop");
            return;
        }
        test_array[idx] = encoder1_count;
        test_array2[idx] = encoder2_count;
        idx++;
        encoder1_count=0;
        encoder2_count=0;
    }    
    //=========================================================//
    mT5ClearIntFlag(); // clear the interrupt flag
    asm("nop");
 }
void pwm1_init(){
    OC1CON = 0x0000; // PWM off
    T2CON  = 0x0000; // Timer 2 w/ prescaler of 64
    OC1R   = 0x000; // Original duty cycle 
    OC1RS  = 0x000; // OC1RS is used to load new duty cycles!
    OC1CON = 0x0006; // PWM Mode
    PR2    = 0x00AF; // Timer's period
    T2CONSET  = 0x8020; // start Timer2
    OC1CONSET = 0x8000; // start PWM
 }
void pwm2_init(){
    OC2CON = 0x0000; // PWM off
    T3CON  = 0x0000; // Timer 3 w/ prescaler of 64
    OC2R   = 0x000; // Original duty cycle 
    OC2RS  = 0x000; // OC2RS is used to load new duty cycles!
    OC2CON = 0x0006; // PWM Mode
    PR3    = 0x00AF; // Timer3's period
    T3CONSET  = 0x8020; // start Timer3
    OC2CONSET = 0x8000; // start PWM
 }
void gpio_init(){	

    AD1PCFG = 0xFFFF;   // ALL digital

    TRISB = 0x00;                               
    TRISD = 0x80;
  
    // Encoder inputs
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    
    // ISD5116 A0-A1 outputs (grounded)
    TRISBbits.TRISB2 = 0; PORTBbits.RB2=0;
    TRISBbits.TRISB3 = 0; PORTBbits.RB3=0;

    // Motor direction outputs
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;

    // Ultrasonic TRIGGER outputs
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB8 = 0;
    
    // Ultrasonic ECHO inputs
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB9 = 1;
    
    // Pushbuttons inputs
    TRISBbits.TRISB10 = 1; 
    TRISBbits.TRISB11 = 1; 
    TRISBbits.TRISB12 = 1; 
    
    // ISD5116 SDA/SCL inputs
    TRISGbits.TRISG2 = 1;
    TRISGbits.TRISG3 = 1;
}
void Initializations(){
    /*=========     GPIO      ===========*/
    gpio_init();
    /*=========     TIMER1    ===========*/
    //ultrasonic_init();
    /*=========     TIMER4   (PUSHBUTTON) ===========*/
    pushbutton_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    //timer5_init();
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    pwm1_init(); // Output is RD0
    pwm2_init(); // Output is RD1
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
void single_beep(){
        char i;
        for(i=0;i<100;i++){
            LATAbits.LATA3 = 1;
            ShortDelay(300*US_TO_CT_TICKS);
            LATAbits.LATA3 = 0;
            ShortDelay(700*US_TO_CT_TICKS);
            asm("nop");
        }
        
    }
/* 0x5F on PWM = 2A motor encoders per half second*/
int main(void)
{       
    unsigned char ob_1 = 0, ob_2 = 0;
    int i = 0;
   
    UINT16 oc1_vals[10] = {0x6F, 0x7F, 0x8F, 0x9F, 0x6F, 0x7F, 0x8F, 0x9F, 0x6F, 0x7F};
    UINT16 oc2_vals[10] = {0x6F, 0x7F, 0x8F, 0x9F, 0x6F, 0x7F, 0x8F, 0x9F, 0x6F, 0x7F};
    
    /* RUN INITIALIZATIONS*/
    Initializations();

    mode=IDLE;
    LATAbits.LATA2 = 0;
    LATAbits.LATA3 = 0;
    
    
    // initialize test_array
    //for(i=0;i<100;i++){test_array[i]=0xFF; test_array2[i] = 0xFF;}
    
    i=0;
    LATBbits.LATB4 = 0; LATBbits.LATB5 = 0; OC1RS = 0x00; OC2RS = 0x00;
    delay_seconds(1);
    //mode = MOTOR_PSSC;
    
    LATAbits.LATA2 = 1; 
    //while(1){}
    
    while(1){
        /*========= CHECK OBSTACLE =============*/
    /*
        ob_2 = Obstacle_2;
        if(ob_2){
            OC1RS =0x00;OC2RS=0x00; 
            LATAbits.LATA2 = 1;     
            single_beep();
                 continue;
        }
        else{LATAbits.LATA2 = 0;}
    */
        /*====== OBSTACLE-FREE =================*/
        if (mode == MOTOR_PSSC){
            
            OC1RS = path;
            OC2RS = oc2_vals[i];
            ShortDelay(500000*US_TO_CT_TICKS);
            i = (i+1);
            if (i==10){mode = IDLE;}
        }
        /*
        // Obstacle - Free!
        else if (mode==RECORDING_PATH){
            OC1RS = 0x00; OC2RS = 0x00;
        }
         */
        else if (mode==PLAYBACK_PATH){
            // get PWM from encoder array   
            for (i=0; i < 10; i++){
            OC1RS = paths[path][i];
            OC2RS = paths[path][i];
            ShortDelay(100000*US_TO_CT_TICKS);
            }
            mode = IDLE;
        }
        // kill motor if not testing   
        
         else{i = 0; OC1RS=0; OC2RS = 0x00;}
    }

// 5F: 2A
    // 96:42
    // max: 
    
} // end main
   













// ULTRASONIC TEST LOOP
    
    /*
    while(1) {
        delay_seconds(2);
        single_beep();
        LATBbits.LATB4 = 0;
    }
     */
    /*
    while(1){
        PORTBbits.RB8 = 1;
        if(Obstacle_2){PORTAbits.RA3 = 1;}
        else{PORTAbits.RA3 = 0;};
        
    }
    */
    //========NVM TESTING====================
/*
    unsigned char page_buff[NVM_PAGE_SIZE];
    unsigned char data_buff[NVM_ROW_SIZE];
    for(i=0; i<NVM_ROW_SIZE; i++){data_buff[i]=i;}
    if(NVMErasePage((void *)NVM_PROGRAM_PAGE)){
        asm("nop");
    };
    delay_seconds(1);
    if(NVMWriteRow((void *)NVM_PROGRAM_PAGE, (void*)data_buff)){
        asm("nop");
    };
    delay_seconds(1);
    if(memcmp(data_buff, (void *)NVM_PROGRAM_PAGE, sizeof(data_buff))){X = 0x23;}
    
    while(1){
        asm("nop");
    asm("nop");asm("nop");asm("nop");
    asm("nop");
    }
    */
    //======================================
    

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
    

//===================== MOTOR functions for Testing==================
/*
void motor1_off(){
    PORTBbits.RB4 = 0;
    PORTBbits.RB5 = 0;
    OC1RS = 0x00;
}
void motor1_on(unsigned char speed){
    PORTBbits.RB4 = 1;
    PORTBbits.RB5 = 1;
    OC1RS = 0x80;
}
void motor2_off(){
    PORTBbits.RB5 = 0;
    OC2RS = 0x00;
}
void motor2_on(unsigned char speed){
    PORTBbits.RB5 = 1;
    OC2RS = speed;
}
 */
//====================================================================