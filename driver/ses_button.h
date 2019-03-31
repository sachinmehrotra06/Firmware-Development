#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

typedef void (*pButtonCallback)(void);
void button_setRotaryButtonCallback(pButtonCallback callback);
void button_setJoystickButtonCallback(pButtonCallback callback);

void button_checkState();

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

#endif /* SES_BUTTON_H_ */
