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
UINT32 test_array[10],test_array_2[10], playback_array[10],playback_array2[10];
volatile int idx = 0;
volatile UINT32 count = 1;
char X = 1;
//UINT8 Path_1[500]; // possible PATH array! attemting to use const for nvm!


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
    
    unsigned char ob_2 = Obstacle_2;
    //Not in a path  mode
    //if(mode==IDLE){toggle = 0; count = 1; mT5ClearIntFlag(); return;}
    if (!( (mode==RECORDING_PATH) || (mode==PLAYBACK_PATH) ) ){
        toggle = 0; count = 1; mT5ClearIntFlag(); return;
    }
    // Don't deal w/ encoder/timer if obstacle detected, OC taken care of in Main!
    if(ob_2){mT5ClearIntFlag();return;}
    //==============================ENCODER LOGIC========================//
    // Increment encoder_val if bit has been toggled!
    if (PORTAbits.RA0 != toggle){
        encoder_count++;
    }    
    // Update toggle value 
    toggle = PORTAbits.RA0;
    
    // Record count in memory
    count = (++count) % 5000;
    if(count==0){
        //we only have 10 spaces in array atm
        if(idx>=10){
            idx = 0;
            count = 1;
            if(mode == PLAYBACK_PATH){
                mode = PLAYBACK_VOICE;
                return;
            }
            mode = RECORDING_VOICE;
            
            asm("nop");
            return;
        }
        if(mode==RECORDING_PATH)test_array[idx] = encoder_count;
        if(mode==PLAYBACK_PATH) playback_array[idx] = encoder_count;
        idx++;
        encoder_count=0;
    }    
    //=========================================================//
    mT5ClearIntFlag(); // clear the interrupt flag
    asm("nop");
 }

void pwm1_init(){
    OC1CON = 0x0000; // PWM off
    T2CON  = 0x0000; // Timer 2 w/ prescaler of 64
    OC1R   = 0x001; // Original duty cycle 
    OC1RS  = 0x002; // OC1RS is used to load new duty cycles!
    OC1CON = 0x0006; // PWM Mode
    PR2    = 0x00AF; // Timer's period
    T2CONSET  = 0x8020; // start Timer2
    OC1CONSET = 0x8000; // start PWM
 }
void pwm2_init(){
    OC2CON = 0x0000; // PWM off
    T3CON  = 0x0000; // Timer 3 w/ prescaler of 64
    OC2R   = 0x001; // Original duty cycle 
    OC2RS  = 0x002; // OC2RS is used to load new duty cycles!
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
    //pushbutton_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    //timer5_init();
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    //pwm1_init(); // Output is RD0
    //pwm2_init(); // Output is RD1
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

/* 0x5F on PWM = 2A motor encoders per half second*/
int main(void)
{       
    unsigned char ob_1 = 0, ob_2 = 0;
    int i = 0;
   int  oc_values[10] = {0x3F, 0x2F, 0x1F, 0x1F, 0x2F, 0x3F, 0x2F, 0x1F, 0x0F, 0x0F};    
   
    /* RUN INITIALIZATIONS*/
    Initializations();

    /* TESTING PWM + ENCODER LOOP*/
    
    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
    //while(1);
    for(i=0;i<10;i++){test_array[i]=0x00; test_array_2[i] = 0x00;} // initialize test_array
    i=0;
    PORTBbits.RB4 = 0; OC1RS = 0x00; OC2RS = 0x00;
    
    delay_seconds(1);
    // ULTRASONIC TEST LOOP
    
    void beep(UINT8 number_of_beeps){
        
        LATBbits.LATB4 = 1;
        ShortDelay(250*US_TO_CT_TICKS);
        LATBbits.LATB4 = 0;
        ShortDelay(750*US_TO_CT_TICKS);
        asm("nop");
        
    }
    
    
    while(1) {
        
    }
    
    /*
    while(1){
        PORTBbits.RB8 = 1;
        if(Obstacle_2){PORTAbits.RA3 = 1;}
        else{PORTAbits.RA3 = 0;};
        
    }
    */
    //========NVM TESTING====================
    //#define NVM_PROGRAM_PAGE 0x1d007000
/*
#define NVM_PROGRAM_PAGE 0xbd008000
    #define NVM_PAGE_SIZE	4096
    #define NVM_ROW_SIZE 512
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
    *///===========================
    
    
    
    
    mode=RECORDING_PATH;
    while(1){
        ob_2 = Obstacle_2;
        if(ob_2){OC1RS =0x00;OC2RS=0x00;PORTAbits.RA3 = 1; continue;}
        
        // Obstacle - Free!
        PORTAbits.RA3 = 0;
        if (mode==RECORDING_PATH){
            PORTBbits.RB4 = 0;
            OC1RS = oc_values[idx];
            OC2RS = oc_values[idx];
        }
        else if (mode==PLAYBACK_PATH){
            // get PWM from encoder array
            PORTBbits.RB4 = 0;
            OC1RS = test_array[idx]  * ENCODER_MULTIPLIER;
            OC2RS = test_array_2[idx] * ENCODER_MULTIPLIER;
        }
        else if (mode == RECORDING_VOICE){
             PORTBbits.RB4 = 0;
            OC1RS = 0x00; OC2RS = 0x00;
            delay_seconds(1);
            mode = PLAYBACK_PATH;i=0;
        }
        // kill motor if not testing   
        else{i = 0; OC1RS=0; OC2RS = 0x00; PORTBbits.RB4 = 0;}
    }

// 5F: 2A
    // 96:42
    // max: 
    
} // end main
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
    
