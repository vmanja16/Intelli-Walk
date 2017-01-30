#include <plib.h>        /* PIC32 peripheral library */

#ifndef _DMAC
    #error "This example needs a PIC32MX processor with DMA controller present. Aborting build!"
#endif  // _DMAC


//  Configuration Bit settings
//  System Clock = 80 MHz,  Peripherial Bus = 40 MHz
//  Primary Osc w/PLL (XT+,HS+,EC+PLL)
//  Input Divider    2x Divider
//  Multiplier      20x Multiplier
//  WDT disabled
//  Other options are don't care
//
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF

#define	SYS_FREQ	80000000		// frequency we're running at





#define	LED_BLINK_RATE	200			// LED blinking rate, in msec

// type in the bytes that define the pattern that will be output to the LED's
// no longer than DmaGetMaxTxferSize() bytes: this is the maximum DMA transfer size
static const unsigned char	LED_pattern[]=
{
	0x1,	0x2,	0x4,	0x8,	0x10,	0x20,	0x40,	0x80,
	0xff,	0x0,	0x55,	0xaa,	0xe7,	0x18,	0xff,	0xaa,
	0x0f,	0x00,	0x0f,	0x00,	0xf0,	0x00,	0xf0,	0x00
};


int main(void)
{
	int	dmaChn=0;		// the DMA channel to use

	// first let us set the LED I/O ports as digital outputs
	mPORTASetPinsDigitalOut(0xff);
	mPORTAClearBits(0xff);		// start with all LED's turned off

	// Open the desired DMA channel.
	// We enable the AUTO option, we'll keep repeating the same transfer over and over.
	DmaChnOpen(dmaChn, 0, DMA_OPEN_AUTO);

	// set the transfer parameters: source & destination address, source & destination size, number of bytes per event
	DmaChnSetTxfer(dmaChn, LED_pattern, (void*)&LATA, sizeof(LED_pattern), 1, 1);

	// set the transfer event control: what event is to start the DMA transfer
	DmaChnSetEventControl(dmaChn, DMA_EV_START_IRQ(_TIMER_3_IRQ));

	// once we configured the DMA channel we can enable it
	// now it's ready and waiting for an event to occur...
	DmaChnEnable(dmaChn);


	// now use the 32 bit timer to generate an interrupt at the desired LED_BLINK_RATE
	{
		int	pbFreq=SYS_FREQ/(1<<mOSCGetPBDIV());	// get the PB frequency the timer is running at

		// use 1:1 prescaler for max resolution, the PB clock
		OpenTimer23(T2_ON | T2_SOURCE_INT | T2_PS_1_1, (pbFreq/1000)*LED_BLINK_RATE);
	}


	while(1)
	{
		// do some other useful work
	}


}
