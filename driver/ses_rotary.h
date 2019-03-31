

#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_


/*INCLUDES *******************************************************************/

#include "ses_common.h"
#include "ses_timer.h"


/*PROTOTYPES *****************************************************************/

/*Initialize the rotary */
void rotary_init();

/**
 * callback for clockwise
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback callback);

/**
 * callback for counter clockwise
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback);

/**
 * Rotary debouncing function
 */
void rotary_checkState();

#endif /* SES_ROTARY_H_ */
