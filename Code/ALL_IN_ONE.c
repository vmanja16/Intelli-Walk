//========================= FOR TESTING main solo! =============================
#include <plib.h>
#include <xc.h>          /* XC32 Compiler Library    */
#include <string.h>
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF
#define SYS_FREQ                  80000000         // frequency we're running at
#define CPU_CLOCK_HZ             (80000000UL)      // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)      // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)      // Peripheral Bus  in Hz 
#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)      // uS to CoreTimer Ticks 
#define ULTRA_THRESHOLD_2 25
#define PAGE_ADDRESS 0xbd008000
//============================================================================
typedef enum {IDLE, RECORDING_PATH, RECORDING_VOICE, PLAYBACK_VOICE, PLAYBACK_PATH} op_mode;
typedef enum {OBSTACLE_DETECTED, POSSIBLE_OBSTACLE, POSSIBLE_CLEAR, FULL_CLEAR} ultrasonic_state;
//==== GLOBAL VARIABLES===============================
volatile UINT8 paths[8][512];
volatile op_mode mode = IDLE;
volatile unsigned int playback_path = 0, record_path = 0, idx = 0;
volatile UINT8 Obstacle_2 = 0;

//====================================================
//=============== HELPER FUNCTIONS====================
void writePathToFlash(UINT8 path_number){
    UINT32 addr = PAGE_ADDRESS + (path_number * 512);
    if(NVMWriteRow((void *)addr, (void*)paths[path_number])){asm("nop");}ShortDelay(10000);
}
void ShortDelay(UINT32 DelayCount){ 
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
//=============================================
//===========PERIPHERALS=======================
void __ISR(_TIMER_4_VECTOR, ipl3) _Timer4Handler(void){
    static unsigned char pb1_status = 1, pb2_status = 1, pb3_status = 1;
    static op_mode next_mode = IDLE;
    static unsigned int next_playback_path = 0;
    static is_reset = 0;
    int i = 0;
    unsigned char b10 = PORTBbits.RB10;
    unsigned char b11 = PORTBbits.RB11;
    unsigned char b12 = PORTBbits.RB12;
    UINT8 ob_2 = Obstacle_2;
    // ALL IN ONE
    static UINT8 counter = 0;
    
    counter = (counter+1) % 6;
    // PB LOGIC if counter==0
    if(counter==0)
    {
        if(is_reset){
            NVMErasePage((void *)PAGE_ADDRESS); ShortDelay(10000);
            is_reset=0; quick_beep();single_beep(); single_beep_high();
        }
        if( (!b11) && (!b12) && (!b10)){is_reset=1;}
        else{is_reset=0;}
        
        // ON PB1 PRESS
        if((pb1_status == 0) && b10){
            LATAbits.LATA2 = !LATAbits.LATA2;
            if(mode==IDLE){next_mode = RECORDING_PATH; single_beep();}
          }
        // ON PB_2 PRESS: Choose a mode from IDLE
        else if((pb2_status == 0) && b11){
            LATAbits.LATA2 = !LATAbits.LATA2;
       
            
            if (mode == IDLE){
                // Update next_playback if exists
            /*DEBUG===============*/
                //mode = PLAYBACK_PATH;
                /*===================*/
                for(i=0;i<8;i++){
                    next_playback_path = (next_playback_path+1) % 8;
                    if(paths[next_playback_path][0]!=0xFF){break;}
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
                playback_path = next_playback_path;
                mode = next_mode;
            }
            else if(mode==RECORDING_PATH){
                mode = IDLE;
                writePathToFlash(record_path);
                record_path++; 
                single_beep_high();
                next_playback_path = 0;
                next_mode = IDLE;
            }
            else if(mode==PLAYBACK_PATH){mode = IDLE; quick_beep();}
        }
        // update button status
        pb1_status = b10;
        pb2_status = b11;
        pb3_status = b12;
    }
    //==============================ENCODERS=======================
    if(ob_2){mT4ClearIntFlag();return;}
    UINT8 a1 = PORTAbits.RA1;
    UINT8 a0 = PORTAbits.RA0;
    // Grey Counter bits
    static UINT8 toggle1 = 0, toggle2 = 0;
    static UINT32 count = 1;
    static UINT8 encoder1_count = 0;
    static UINT8 encoder2_count = 0;

    // for playback, just deal w/ count and index
    if(mode==PLAYBACK_PATH){
        // increment playback count
        count = (++count)%500;
        if(count==0){idx++;} 
        if(paths[playback_path][idx]==0xFF){ single_beep();single_beep_high();mode = IDLE;}
    }
    // Non Record/Playback --> dont care!
    else if (mode!=RECORDING_PATH){
        count = 1; idx=0; encoder1_count = 0; encoder2_count=0;
    }
    else{
        // Record count in memory while recording
        count = (++count) % 500;

        //==============================ENCODER LOGIC========================//
        // Increment encoder_val if bit has been toggle1d!
        if (a1 != toggle1){encoder1_count++;}
        if (a0 != toggle2){encoder2_count++; LATAbits.LATA2 = !LATAbits.LATA2;}
        // Update toggle1 value 
        toggle1 = a1;
        toggle2 = a0;

        if(count==0){
            //we only have 100 spaces in array atm
            paths[record_path][idx] = encoder1_count;
            paths[record_path][idx+256] = encoder2_count;
            idx++;
            if(idx > 250){
                    // Update record_path number, goto IDLE
                    record_path++;
                    single_beep_high();
                    mode = IDLE; next_mode = IDLE; idx=0;
                }
            encoder1_count=0;
            encoder2_count=0;
        }
    }
     
    mT4ClearIntFlag(); // clear the interrupt flag
}

void pushbutton_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = 0x0F42; // Load the period register 
    T4CONSET = 0x8000; // Start the timer
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_3);// interrupt with a Priority of 3 and zero sub-priority
}
/* ULTRASONIC POLLING TIMER INITIALIZATION*/
void ultrasonic_init(void){
    T1CON = 0x0000; // Stop the timer and clear the control register,
    TMR1 = 0x0000; // Clear the timer register
    PR1 = 0x0191; // Load the period register with value 400, since 400 ticks = 10 microsecond
    T1CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a Priority of 2 and zero sub-priority
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2);
}
void __ISR(_TIMER_1_VECTOR, ipl2) _Timer1Handler(void){
    
    static char edge_2_flag = 0;
    static ultrasonic_state ob_2_state= FULL_CLEAR;
    static UINT8 Echo2_count = 0;
    unsigned char ultra_val = PORTBbits.RB9;
    // reset echo count and update state on rising edge
    if ((edge_2_flag==0) && (ultra_val==1) ){
        if (Echo2_count < ULTRA_THRESHOLD_2 ){
         //    Obstacle_2 = 1; // Obstacle detected
             if( (ob_2_state==OBSTACLE_DETECTED) || (ob_2_state==POSSIBLE_OBSTACLE) ){
                 ob_2_state = OBSTACLE_DETECTED;
             }
             else{ob_2_state = POSSIBLE_OBSTACLE;}
         }
         else{
         //    Obstacle_2 = 0; // No obstacle
             if( (ob_2_state==FULL_CLEAR) || (ob_2_state==POSSIBLE_CLEAR) ){
                 ob_2_state= FULL_CLEAR;
             }
             else{ob_2_state = POSSIBLE_CLEAR;}
         }
        Echo2_count = 0;
    }
    
    // Increment Echo_count!
    if (ultra_val){Echo2_count ++;}
 
         // OUTPUT LOGIC
        if(ob_2_state==FULL_CLEAR){Obstacle_2 = 0;}
        if(ob_2_state==OBSTACLE_DETECTED){Obstacle_2=1;}
    
    edge_2_flag = ultra_val; // update prev edge status
    mT1ClearIntFlag(); // clear the interrupt flag
    
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

void flash_init(){
    memcpy(paths, (void *) PAGE_ADDRESS,4096);
}
void gpio_init(){   

    AD1PCFG = 0xFFFF;   // ALL digital

    TRISB = 0x00;                               
    TRISD = 0x80;
  
    // Encoder inputs
    TRISBbits.TRISB8 = 1;
    TRISAbits.TRISA0 = 1; // pulled out
    TRISAbits.TRISA1 = 1;
    
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    
    // ISD5116 A0-A1 outputs (grounded)
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;

    // Motor direction outputs
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;

    // Ultrasonic TRIGGER outputs
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    
    // Ultrasonic ECHO inputs
    
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
    /*=========     PWM (TIMERS 2 and 3)      ===========*/
    pwm1_init();
    pwm2_init();
    /*=========     TIMER4   (PUSHBUTTON) ===========*/
       pushbutton_init();
    /*=========     TIMER5  (ENCODERS)  ===========*/
    //ultrasonic_init();
    flash_init();
    /*=========== Interrupts    ========*/
    INTEnableSystemMultiVectoredInt();
}
int main(void){       
    int i = 0, j=0;
    UINT8 left_pow, right_pow;
    op_mode curr_mode = mode;
    // Initialize EOP flags
    /*
    for(i=0; i< 8; i++){
        for(j=0; j < 512; j++){
            paths[i][j] = 0xFF;
        }
    }
     */
    /* RUN INITIALIZATIONS*/
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
    // Initial Beeping Sequence 
    single_beep();quick_beep();single_beep_high();quick_beep();single_beep_high();single_beep();
    
    //while(1){OC1RS=0x8F; OC2RS = 0x8F;}
    while(1){
        
        // ULTRASONICS
        while(Obstacle_2){
            
            OC1RS=0x00; OC2RS=0x00;
            quick_beep();ShortDelay(1000);quick_beep();ShortDelay(1000);quick_beep();ShortDelay(1000);
        }
        j = idx+256;
        if (mode==RECORDING_PATH){OC1RS = 0x0;OC2RS = 0x0;}
        else if (mode==PLAYBACK_PATH){   
            
            left_pow = paths[playback_path][j];
            right_pow = paths[playback_path][idx];
            if(left_pow > 5){left_pow =0x8F ;}else{left_pow*=45;}
            if(right_pow > 5){right_pow = 0x8F;}else{right_pow*=45;}
            OC1RS = right_pow;
            OC2RS = left_pow;
             
            //OC1RS = 0x7F; OC2RS = 0x7F;
        }
         else{OC1RS=0x00; OC2RS = 0x00;}
    }
} // end main
