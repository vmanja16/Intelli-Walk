#include <plib.h>        /* PIC32 peripheral library */
#include <stdio.h>
#include <xc.h>

// stuff for shortdelay function
#define CPU_CLOCK_HZ             (80000000UL)    // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)    // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)    // Peripheral Bus  in Hz 

#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)    // uS to CoreTimer Ticks 
#define MS_50 5000
#define ULTRA_THRESHOLD_1 200
#define ULTRA_THRESHOLD_2 400
//  Configuration Bit settings
//  System Clock = 80 MHz,  Peripherial Bus = 40 MHz
//  Primary Osc w/PLL (XT+,HS+,EC+PLL)
//  Input Divider    2x Divider
//  Multiplier      20x Multiplier
//  Other options are don't care
//
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF

#define	SYS_FREQ	80000000		// frequency we're running at

// Define GLOBAL VARIABLES HERE
unsigned int Timer1_count = 0; // Number of Timer 1 Interrupts 
unsigned int Echo1_count = 0;  // Number of Timer1 interrupts in echo1
unsigned int Echo2_count = 0;  // Number of Timer1 interrupts in echo2
char Obstacle_1 = 0; // Boolean signifying Ultra1 found an obstacle
char Obstacle_2 = 0;


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
                mPORTASetBits(0x0F);
                Obstacle_1 = 1; // Obstacle detected
                OC1RS = 0x0000; // shutoff the PWM
            }
            else{
                mPORTAClearBits(0x0F);
                Obstacle_1 = 0; // No obstacle
            }
            Echo1_count = 0; // reset Echo count
        }
    }
    if (PORTBbits.RB1){Echo2_count ++;}
    else{
        if (Echo2_count > 5){
            if (Echo2_count < ULTRA_THRESHOLD_2 ){
                mPORTASetBits(0xF0);
                Obstacle_2 = 1; // Obstacle detected
                OC1RS = 0x0000; // shutoff the PWM
            }
            else{
                mPORTAClearBits(0xF0);
                Obstacle_2 = 0; // No obstacle
            }
            Echo2_count = 0; // reset Echo count
        }
    }
    mT1ClearIntFlag(); // clear the interrupt flag
    
 }

void configureTimer1(){
    T1CON = 0x0000; // Stop the timer and clear the control register,
    TMR1 = 0x0000; // Clear the timer register
    PR1 = 0x0190; // Load the period register with value 400, since 400 ticks = 10 microsecond
    T1CONSET = 0x8000; // Start the timer
    // set up the core timer interrupt with a prioirty of 2 and zero sub-priority
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2);
 }
void configurePWM1(){
    OC1CON = 0x0000; // PWM off
    T2CON  = 0x0000; // Timer 2 w/ prescaler of 64
    OC1R   = 0x0064; // Original duty cycle 
    OC1RS  = 0x0064; // OC1RS is used to load new duty cycles!
    OC1CON = 0x0006; // PWM Mode
    PR2    = 0x00AF; // Timer's period
    T2CONSET  = 0x8020; // start Timer2
    OC1CONSET = 0x8000; // start PWM
 }
void ShortDelay(UINT32 DelayCount){                   // Delay Time (CoreTimer Ticks)  
  UINT32 StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime 
  while ((UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {} ;
} 

 
int main(void)
{   
    mPORTASetPinsDigitalOut(0xff);              // set LEDS as digital outputs
	mPORTAClearBits(0xff);                      // clear LEDS
    	
    AD1PCFG = 0b0010011;                        // RB 0,1,4, 7 set to digital pins
    TRISB = 0x03;                               // RB 0,1 as Input PIN (ECHO 1,2)                                // RB4 is Output PIN (TRIGGER)  
    TRISD = 0x80;                               // LCD Pins as Output
    
    /*============ PWM ===================     */
    configurePWM1();
    // Output is RD0    
    int i = 0;
    int  oc_values[10] = {0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F, 0x9F};
    /*==========================================================*/    
    
    /*=========    TIMER1     ===========*/
    configureTimer1();
    /*==========================================================*/
    // enable device multi-vector interrupts
    INTEnableSystemMultiVectoredInt();
    
    /*    MAIN WHILE LOOP*/
    while(1)
    {
            ShortDelay(50000 * US_TO_CT_TICKS) ;
        if (!(Obstacle_1||Obstacle_2)){
            i = (i + 1) % 10;
            OC1RS = oc_values[i];
        }
    }
    
}
