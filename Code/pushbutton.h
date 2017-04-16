
#ifndef _PUSHBUTTON_H    /* Guard against multiple inclusion */
#define _PUSHBUTTON_H

#include <plib.h>
#include <xc.h>
#include "settings.h"

extern volatile op_mode mode;

#define PUSH_BUTTON_1 PORTBbits.RB10 
#define PUSH_BUTTON_2 PORTBbits.RB11
#define PUSH_BUTTON_3 PORTBbits.RB12
#define TRIS_PUSH_BUTTON1 TRISBbits.TRISB10; // and its tris reg val
#define TRIS_PUSH_BUTTON2 TRISBbits.TRISB10; // and its tris reg val
#define TRIS_PUSH_BUTTON3 TRISBbits.TRISB10; // and its tris reg val
#define SAMPLING_PERIOD (0x0F43 << 1) // time between checking status of button


#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
