#include <plib.h> // Peripheral library
#define PUSH_BUTTON_1 PORTDbits.RD10 // choose an input
#define SAMPLING_PERIOD 0x0F42 // time between checking status of button
// compiler pragmas
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF

void timer4_init(void){
    T4CON = 0x0000; // Stop the timer and clear the control register,
    TMR4 = 0x0000; // Clear the timer register
    PR4 = SAMPLING_PERIOD; // Load the period register with value 4000, since 4000 ticks = 100 microsecond
    T4CONSET = 0x8020; // Start the timer
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_3);// interrupt with a Priority of 3 and zero sub-priority
}

void __ISR(_TIMER_4_VECTOR, ipl3) _Timer4Handler(void){

    // static vars keep their value between interrupts
    static unsigned char pb_status = 1; // 0  = pushed (or not depending on circuit)

    // Toggle LEDs while pressed
    if(pb_status != PUSH_BUTTON_1){
        mPORTAToggleBits(0xFF);
    }

    // update button status
    pb_status = PUSH_BUTTON_1;
    mT4ClearIntFlag(); // clear the interrupt flag
 }

// Standard Main Function
 int main(){
    mPORTAClearBits(0xFF); // Clear LEDs
    AD1PCFG = 0xFFFF;   // ALL digital I/O
    TRISDbits.TRISD10 = 1; // Pushbutton is input
    timer4_init(); // Initialize timer for sampling
     INTEnableSystemMultiVectoredInt(); // allow interrupts
    while(1); // LOOP
}
