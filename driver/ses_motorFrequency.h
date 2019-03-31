#ifndef SES_MOTORFREQUENCY_H_
#define SES_MOTORFREQUENCY_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_pwm.h"
#include "ses_button.h"
#include "ses_lcd.h"
#include "ses_uart.h"
#include "ses_timer.h"
#include "ses_led.h"
#include "ses_scheduler.h"
#include "ses_adc.h"
#include "util/atomic.h"

/* FUNCTION PROTOTYPES *******************************************************/

void motorFrequency_init();

uint16_t motorFrequency_getRecent();

uint16_t motorFrequency_getMedian();

#endif	/* SES_MOTORFREQUENCY_H_ */
