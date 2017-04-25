
#ifndef _PUSHBUTTON_H    /* Guard against multiple inclusion */
#define _PUSHBUTTON_H

#include <plib.h>
#include <xc.h>
#include "settings.h"
#include "ports.h"

extern volatile op_mode mode;
extern volatile int idx;
extern volatile int path;

#define SAMPLING_PERIOD 0x0190 // time between checking status of button


#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
