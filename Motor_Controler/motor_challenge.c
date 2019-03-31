/* INCLUDES ******************************************************************/

#include "ses_pwm.h"
#include "ses_motorFrequency.h"
#include "ses_uart.h"
#include "ses_lcd.h"
#include "ses_scheduler.h"
#include "ses_common.h"
#include "ses_button.h"

/* DEFINES & MACROS **********************************************************/

#define DUTY_STOP	0

/* GLOBAL VARIABLES & ARRAYS **************************************************/

taskDescriptor* rpmCount;
taskDescriptor* plot;

static uint16_t i = 0;
uint16_t frequency = 0;
uint16_t duty = 0;

//Declaring variables for PID algorithm
static uint16_t ft = 160, aw = 10000;
static int kp = 0, kd = 0, ki = 1;
static int e = 0, elast = 0, y = 0, ew = 0;

/* FUNCTION PROTOTYPES *******************************************************/

void plotFrequency();
void rpm_function();
uint16_t controller_PID();

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

	plot = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	plot->task = &plotFrequency;
	plot->period = 1000;
	plot->expire = rpmCount->period;
	plot->execute = 0;
	plot->param = NULL;
	plot->reserved = 0;
	plot->next = NULL;

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
	scheduler_add(plot);

	//for pwm init on timer/counter0
	pwm_init();

	//for generating external interrupt at pin INT0
	motorFrequency_init();

	//Stopping motor initially
	pwm_setDutyCycle(DUTY_STOP);

	sei();

	while(true)
		scheduler_run();


	return 0;
}

/* FUNCTION DEFINITION *******************************************************/

//To print frequency and rpm every second
void rpm_function()
{
	frequency = motorFrequency_getRecent();

	lcd_setCursor(0, 0);
	fprintf(lcdout, "%u\t%u", frequency, duty);

}

//To plot the frequency every second
void plotFrequency()
{
	i++;

	//Calling controller to record duty cycle values and using it to run motor
	duty = controller_PID();
	pwm_setDutyCycle(duty);

	if(i != 5000)
		lcd_setPixel(frequency/10, i, true);
	else
		i = 0;
}

//Implementation of PID controller
uint16_t controller_PID()
{
	//target freq to maintain

	int temp1 = 0, temp2 = 0;

	e = ft - frequency;
	temp1 = ew + e;

	//min
	if(temp1 < aw)
		temp2 = temp1;
	else
		temp2 = aw;

	//max
	if(temp2 < -aw)
		ew = -aw;
	else
		ew = temp2;

	y = kp * e + ki * ew + kd * (elast - e);

	elast = e;

	return y;

}
