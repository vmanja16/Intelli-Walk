#include <plib.h>        /* PIC32 peripheral library */
#include <stdio.h>
#include <xc.h>

#ifndef _DMAC
    //#error "This exam//  WDT disabled ple needs a PIC32MX processor with DMA controller present. Aborting build!"
#endif  // _DMAC

// stuff for shortdelay function
#define CPU_CLOCK_HZ             (80000000UL)    // CPU Clock Speed in Hz 
#define CPU_CT_HZ            (CPU_CLOCK_HZ/2)    // CPU CoreTimer   in Hz 
#define PERIPHERAL_CLOCK_HZ      (40000000UL)    // Peripheral Bus  in Hz 

#define US_TO_CT_TICKS  (CPU_CT_HZ/1000000UL)    // uS to CoreTimer Ticks 


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


// delay function for TRIGGER
void ShortDelay(                       // Short Delay 
  UINT32 DelayCount)                   // Delay Time (CoreTimer Ticks) 
{ 
  UINT32 StartTime;                    // Start Time 
  StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime 
  while ((UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {}; 
} 


// initialize a timer to make an interrupt every .5 seconds. 
/*void initTimer1(void)
{
    T1CON = 0x0000;         // DISABLE TIMER
    TMR1 = 0x0000;          // clear counter register
    PR1 = 0xFFFF;           // set period
    IPC1 = 0x0000000C;
    IPC1 = 0X00000001;      // Set priority to 3 and subpriority to 1
    IFS0 = 0x00000010;      // clear interrupt flag
    IEC0 = 0x00000100;      // enable interrupts
    T2CON = 0x8000;         // ENABLE TIMER
    

}*/
/*void __ISR(0,ipl3)Timer1Handler(void)
{
    // our code for shaft encoder
    
    
    IFS0 = 0x00000010;                          // clear interrupt flag again
}*/

int main(void)
{
       
    mPORTASetPinsDigitalOut(0xff);              // set LEDS as digital outputs
	mPORTAClearBits(0xff);                      // clear LEDS
    	
    int a;
    AD1PCFG = 0b0010001;                        // RB0, RB4 set to digital pins
    TRISB = 0x01;                               // RB0 as Input PIN (ECHO)
                                                // RB4 is Output PIN (TRIGGER)  
    TRISD = 0x00;                               // LCD Pins as Output
    
    while(1)
    {
        TMR1 = 0x0000;                          // reset TMR1
        PORTB = 0x0010;                         // TRIGGER HIGH (RB4)(use port to write and TRIS TO set INPUT/OUTPUT)
        ShortDelay(10 * US_TO_CT_TICKS);        // Delay 10 uS  
        PORTB = 0x0000;                         // TRIGGER LOW (RB4)
                               
        while(PORTB == 0x0001)                 // ECHO received 
            T1CON = 0x8020;                     // Timer Stops

        T1CON = 0x0000;
        
        a = TMR1;                               // Reads Timer Value

        if(a>=1 && a<=625)                       // Check whether the result is valid or not
        {
            mPORTASetBits(0xff);                // lights up when object detected ~1foot away
        }
        else
            mPORTAClearBits(0xff);
            
    }
    
}
