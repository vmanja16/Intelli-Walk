
#ifndef _PUSHBUTTON_H    /* Guard against multiple inclusion */
#define _PUSHBUTTON_H

#include <plib.h>
#include <xc.h>
#include "settings.h"
#include "ports.h"
#include "ultrasonic.h"

extern volatile op_mode mode;
extern volatile int idx;
extern volatile int path;
extern volatile int playback_path, record_path;
extern volatile UINT8 * paths[10];
#define SAMPLING_PERIOD 0x0190 // time between checking status of button


#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
