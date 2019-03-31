/* INCLUDES ******************************************************************/

#include "ses_pwm.h"
#include "ses_motorFrequency.h"
#include "ses_uart.h"
#include "ses_lcd.h"
#include "ses_scheduler.h"
#include "ses_common.h"
#include "ses_button.h"

/* DEFINES & MACROS **********************************************************/

#define DUTY_CYCLE	170
#define DUTY_STOP	0

/* GLOBAL VARIABLES & ARRAYS *************************************************/

taskDescriptor* rpmCount;
static int button_count = 0;

/* FUNCTION PROTOTYPES *******************************************************/

void call_pwmFunction();
void rpm_function();

/* MAIN FUNCTION *************************************************************/

int main()
{

	//initializing the task for displaying the rpm on display
	rpmCount = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	rpmCount->task = &rpm_function;
	rpmCount->period = 1000;
	rpmCount->expire = rpmCount->period;
	rpmCount->execute = 0;
	rpmCount->param = NULL;
	rpmCount->reserved = 0;
	rpmCount->next = NULL;

	led_redInit();
	led_yellowInit();
	led_greenInit();

	lcd_init();
	uart_init(57600);

	button_init(true);

	led_redOff();
	led_yellowOff();
	led_greenOff();

	//Scheduler init and task addition
	scheduler_init();
	scheduler_add(rpmCount);

	//for pwm init on timer/counter0
	pwm_init();

	//for generating external interrupt at pin INT0
	motorFrequency_init();

	//Initially stopping the motor
	pwm_setDutyCycle(DUTY_STOP);

	//button_setJoystickButtonCallback(&call_pwmFunction);
	button_setRotaryButtonCallback(call_pwmFunction);

	sei();

	while(true)
		scheduler_run();


	return 0;
}


/* FUNCTION DEFINITION *******************************************************/

//Motor control by button press
void call_pwmFunction()
{
	button_count++;

	if(button_count%2 == 1)
	{
		pwm_setDutyCycle(DUTY_CYCLE);
		led_greenOff();
	}
	else if(button_count%2 == 0)
	{
		pwm_setDutyCycle(DUTY_STOP);
		button_count = 0;
	}
}

//Calling this function every second to print freq and rpm of the motor
void rpm_function()
{
	uint16_t rotation = 0;
	uint16_t frequncy = 0;
	lcd_clear();

	//frequency of motor and rotations per one minute
	frequncy = motorFrequency_getRecent();
	rotation = motorFrequency_getMedian() * 60;

	lcd_setCursor(0, 0);
	fprintf(lcdout, "Freq is: %u", frequncy);

	lcd_setCursor(0, 1);
	fprintf(lcdout, "RPM is: %u", rotation);
}
