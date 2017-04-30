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
//UINT16 page_buff[NVM_PAGE_SIZE_BY_2];
//UINT16 data_buff_1[NVM_ROW_SIZE_BY_2], data_buff_2[NVM_ROW_SIZE_BY_2];

//=================== FOR PATH SELECTION DEMO ================================
UINT8 path_0[200], path_1[200], path_2[200], path_3[200], path_4[200], path_5[200], path_6[200], path_7[200], path_8[200],path_9[200];

volatile UINT8 * paths [10] = {path_0, path_1, path_2, path_3, path_4, path_5, path_6, path_7, path_8, path_9};

  //============================================================================
/*
void flash_init(){
    memcpy(page_buff, (void *)NVM_PROGRAM_PAGE_z, sizeof(page_buff));
}
*/
/* MOTOR ENCODER SAMPLER AND SUMMER*/
void encoders_init(void){
    T5CON = 0x0000; // Stop the timer and clear the control register,
    TMR5 = 0x0000; // Clear the timer register
    PR5 = 0x0F42; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T5CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a Priority of 2 and zero sub-priority
    ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_3);
}
void __ISR(_TIMER_5_VECTOR, ipl3) _Timer5Handler(void){
   
    UINT8 a1 = PORTAbits.RA1;
    UINT8 a0 = PORTAbits.RA0;
    
    // Grey Counter bit
    static UINT8 toggle1 = 0, toggle2 = 0;
  //  unsigned char ob_2 = Obstacle_2;
    
    // for playback, just deal w/ count and index
    if(mode==PLAYBACK_PATH){
        // Don't deal w/ encoder/timer if obstacle detected, OC taken care of in Main!
    //    if(ob_2){mT5ClearIntFlag();return;}
        // increment playback count
        if( ( (++count) % 5000)==0){idx++;} 
        mT5ClearIntFlag(); 
        return;
    }
    // Non Record/Playback --> dont care!
    if (mode!=RECORDING_PATH){
        count = 1; idx=0; encoder1_count = 0; encoder2_count=0;mT5ClearIntFlag(); return;
    }
    
    // Record count in memory while recording
    count = (++count) % 5000;
    
    //==============================ENCODER LOGIC========================//
    // Increment encoder_val if bit has been toggle1d!
    if (a1 != toggle1){encoder1_count++;}
    if (a0 != toggle2){encoder2_count++; LATAbits.LATA2 = !LATAbits.LATA2;}
    if(encoder1_count==0xFF){quick_beep;}
    if(encoder2_count==0xFF){quick_beep;}
    // Update toggle1 value 
    toggle1 = a1;
    toggle2 = a0;
    
    if(count==0){
        //we only have 100 spaces in array atm
        paths[record_path][idx] = encoder1_count;
        paths[record_path][idx+100] = encoder2_count;
        idx++;
        if(idx > 95){
                // Update record_path number, goto IDLE
                record_path++;
                single_beep_high();
                mode = IDLE; 
            }
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
    /*============= FLASH ==========================*/
    //int i; for(i=0; i< NVM_ROW_SIZE_BY_2; i++){data_buff_2[i] = 0x23;}
    //if(NVMWriteRow((void *)NVM_PROGRAM_PAGE_z, (void*)data_buff_2)){asm("nop");}ShortDelay(1000);
    //flash_init();
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    pwm1_init();
    pwm2_init();
    /*=========     TIMER1    ===========*/
    //ultrasonic_init();
    /*=========     TIMER4   (PUSHBUTTON) ===========*/
    pushbutton_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    encoders_init();
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
        unsigned char i;
        for(i=0;i<60;i++){
            LATAbits.LATA3 = 1;
            ShortDelay(300*US_TO_CT_TICKS);
            LATAbits.LATA3 = 0;
            ShortDelay(700*US_TO_CT_TICKS);
            asm("nop");
        }
        
    }
void single_beep_high(){
        unsigned char i;
        for(i=0;i<60;i++){
            LATAbits.LATA3 = 1;
            ShortDelay(150*US_TO_CT_TICKS);
            LATAbits.LATA3 = 0;
            ShortDelay(350*US_TO_CT_TICKS);
            asm("nop");
        }
        
    }
void quick_beep(){
     char i;
    for(i=0;i<50;i++){
        LATAbits.LATA3 = 1;
        ShortDelay(75*US_TO_CT_TICKS);
        LATAbits.LATA3 = 0;
        ShortDelay(175*US_TO_CT_TICKS);
        asm("nop");
    }
}
/* 0x5F on PWM = 2A motor encoders per half second*/
int main(void)
{       
    unsigned char ob_1 = 0, ob_2 = 0;
    int i = 0, j=0;
    UINT8 left_pow, right_pow;
    //UINT16 oc1_vals[10] = {0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F};
    //UINT16 oc2_vals[10] = {0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F};
    
    /* RUN INITIALIZATIONS*/
    // Initialize EOP flags
    for(i=0; i< 10; i++){
        for(j=0; j < 200; j++){
            paths[i][j] = 0xFF;
        }
    }
    Initializations();

    mode=IDLE;
    LATAbits.LATA2 = 0;
    LATAbits.LATA3 = 0;
    
    
    i=0; j = 0;
    // Initialize PWM pins
    LATBbits.LATB4 = 0; LATBbits.LATB5 = 0; OC1RS = 0x00; OC2RS = 0x00;
    delay_seconds(1);
    // Light LED
    LATAbits.LATA2 = 1; 
   /* =============== DEBUG CODE==========*/
    //mode = RECORDING_PATH; 
    /*=====================================*/
    while(1){
        j = idx+100;
    
        /*====== OBSTACLE-FREE =================*/
      /*  
        if (mode == MOTOR_PSSC){
            
            OC1RS = oc1_vals[i];
            OC2RS = oc2_vals[i];
            ShortDelay(50000*US_TO_CT_TICKS);
            i = (i+1);
            if (i==10){mode = IDLE;}
        }
        */
        // Obstacle - Free!
   
        if (mode==RECORDING_PATH){
            OC1RS = 0x7F;
            OC2RS = 0x7F;
            
        }
        else if (mode==PLAYBACK_PATH){   
            left_pow = paths[playback_path][j];
            right_pow = paths[playback_path][idx];
            /*========= CHECK OBSTACLE =============*/
        /*
        ob_2 = Obstacle_2;
        if(ob_2){
            OC1RS =0x00;OC2RS=0x00; 
            quick_beep(); ShortDelay(1000*US_TO_CT_TICKS);quick_beep();ShortDelay(1000*US_TO_CT_TICKS);quick_beep();
                 ShortDelay(1000*US_TO_CT_TICKS);continue;
        }
        */
            // EOP found
            if (right_pow==0xFF){ 
                mode = IDLE; single_beep();single_beep_high(); 
                // TURNING OFF ULTRAS!
                //ultrasonic_off();
            }
            else{
                if(left_pow > 0) {OC1RS = left_pow << 1;}else{OC1RS=0x00;}
                if(right_pow > 0){OC2RS = right_pow << 1;}else{OC2RS=0x00;} 
            }
        }
        // kill motor if not testing        
         else{OC1RS=0x00; OC2RS = 0x00;}
    }

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
void write_to_flash(unsigned char * data_buff_1, unsigned char * data_buff_2,int path_num){
    // is data_buff more than two rows?
    int i = 0; unsigned char row_buff [NVM_ROW_SIZE];
    
    // copy page into a buffer
    memcpy(page_buff, (void *)NVM_PROGRAM_PAGE, sizeof(page_buff));
    
    // TODO: copy data_buff into page buff
    
    // Erase page
    if(NVMErasePage((void *)NVM_PROGRAM_PAGE)){asm("nop");};
    ShortDelay(100);
    // Write each row into flash
    for (i=0; i< 8; i++){
        memcpy(row_buff, (const void *) (NVM_PROGRAM_PAGE+((i << 9))), sizeof(row_buff));
        if(NVMWriteRow((void *)NVM_PROGRAM_PAGE, (void*)row_buff)){asm("nop");}
        ShortDelay(100);
    }
    
    //if(memcmp(data_buff, (void *)NVM_PROGRAM_PAGE, sizeof(data_buff))){X = 0x23;}
    //======================================
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