/*INCLUDES ************************************************************/

#include "ses_led.h"
#include "ses_common.h"
#include "ses_scheduler.h"
#include "ses_button.h"
#include "ses_lcd.h"
#include "ses_scheduler.h"
#include "ses_fsm.h"
#include "ses_rotary.h"

/* GLOBAL VARIABLES **************************************************/

taskDescriptor forTime;
taskDescriptor forAlarm;

/* MAIN APPLICATION **************************************************/

int main()
{

	/*Initialize LED */
	led_redInit();
	led_yellowInit();
	led_greenInit();

	/*Initializing Button and rotary rotation */
	button_init(true);
	rotary_init();

	/*Initialize lcd */
	lcd_init();

	led_redOff();
	led_yellowOff();
	led_greenOff();

	/*Initialize scheduler */
	scheduler_init();

	//Adding task to run the clock in scheduler
	forTime.period = 1000;
	forTime.expire = forTime.period;
	forTime.execute = 0;
	forTime.next = NULL;

	forAlarm.period = 250;
	forAlarm.expire = forAlarm.period;
	forAlarm.execute = 0;
	forAlarm.next = NULL;

	/*Initialize FSM and Scheduler Tasks */
	myfsm_init(forTime, forAlarm);

	sei();

	while(true)
		scheduler_run();

	return 0;
}
