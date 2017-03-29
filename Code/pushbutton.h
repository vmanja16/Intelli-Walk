
#ifndef _PUSHBUTTON_H    /* Guard against multiple inclusion */
#define _PUSHBUTTON_H

#include <plib.h>
#include <xc.h>
#include "settings.h"

extern volatile op_mode mode;

#define PUSH_BUTTON_1 PORTDbits.RD10 // choose an input
#define TRIS_PUSH_BUTTON1 TRISDbits.TRISD10; // and its tris reg val
#define SAMPLING_PERIOD 0x0F42 // time between checking status of button


#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
