#ifndef SES_PWM_H_
#define SES_PWM_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_lcd.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_timer.h"

/* FUNCTION PROTOTYPES *******************************************************/

void pwm_init(void);
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif	/* SES_PWM_H */
