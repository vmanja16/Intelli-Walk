#ifndef _PORTS_H    /* Guard against multiple inclusion */
#define _PORTS_H


    // Encoder inputs
	#define LEFT_ENCODER  PORTAbits.RA0
	#define RIGHT_ENCODER PORTAbits.RA1
    
    //TRISAbits.TRISA2 = 0;
    //TRISAbits.TRISA3 = 0;

    #define SPEAKER_PWM LATAbits.LATA3
    
    // ISD5116 A0-A1 outputs (grounded)
    #define ISD_AO_TRIS TRISBbits.TRISB2
    #define ISD_A1_TRIS TRISBbits.TRISB3
    #define ISD_A0      LATBbits.LATB0
    #define ISD_A1      LATBbits.LATB1

    // ISD5116 SDA/SCL inputs
    #define ISD_SCL_TRIS TRISGbits.TRISG2
    #define ISD_SDA_TRIS TRISGbits.TRISG3

    // Motor direction 
    #define LEFT_MOTOR_DIR_TRIS  TRISBbits.TRISB4
    #define RIGHT_MOTOR_DIR_TRIS TRISBbits.TRISB5
    #define LEFT_MOTOR_DIR       LATBbits.LATB4
    #define RIGHT_MOTOR_DIR      LATBbits.LATB5

    
    // Ultrasonic 1 
    #define ULTRASONIC_1_TRIGGER_TRIS TRISBbits.TRISB7
    #define ULTRASONIC_1_TRIGGER      LATBbits.LATB7
    #define ULTRASONIC_1_ECHO_TRIS    TRISBbits.TRISB6
    #define ULTRASONIC_1_ECHO         PORTBbits.RB6
    
    // Ultrasonic 2
    #define ULTRASONIC_2_TRIS         TRISBbits.TRISB9
    #define ULTRASONIC_2              PORTBbits.RB9
    
    // Pushbuttons 
    #define PUSHBUTTON_1_TRIS TRISBbits.TRISB10
    #define PUSHBUTTON_2_TRIS TRISBbits.TRISB11
    #define PUSHBUTTON_3_TRIS TRISBbits.TRISB12
    #define PUSHBUTTON_1      PORTBbits.RB10
    #define PUSHBUTTON_2      PORTBbits.RB11
    #define PUSHBUTTON_3      PORTBbits.RB12


#endif /* _PORTS_H */

/* *****************************************************************************
 End of File
 */
