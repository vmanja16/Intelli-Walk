#include "intelli-walk.h"

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
    	
    AD1PCFG = 0b0010011;                        // RB 0,1,4, 7 set to digital pins
    TRISB = 0x03;                               // RB 0,1 as Input PIN (ECHO 1,2)                                // RB4 is Output PIN (TRIGGER)  
    TRISD = 0x80;                               // LCD Pins as Output
}
void Initializations(){
    /*=========     GPIO      ===========*/
    gpio_init();
    /*=========     TIMER1    ===========*/
    timer1_init();
    /*=========     PWM       ===========*/
    pwm1_init(); // Output is RD0
    /*=========     I2C       ===========*/
    I2CEnable(I2C1, TRUE);
    /*=========== Interrupts    ========*/
    INTEnableSystemMultiVectoredInt();
}
void ShortDelay(UINT32 DelayCount){                   // Delay Time (CoreTimer Ticks)  
  UINT32 StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime 
  while ((UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {} ;
} 

int main(void)
{       
    Initializations();
    int i = 0;
    int  oc_values[10] = {0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F, 0x9F};    
   
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