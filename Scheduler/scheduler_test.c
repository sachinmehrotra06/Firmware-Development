/*
 * main.c
 *
 *  Created on: Apr 19, 2016
 *      Author: manjunath
 */
#include "ses_led.h"
#include "ses_common.h"
#include "ses_scheduler.h"
#include "ses_button.h"
#include "ses_lcd.h"

void task_ledToggle(void *);
void task_pressJoystick();
void call_yellowToggle(void *);
void task_stopWatch(void *);
void initialize_aperiodicTask();

taskDescriptor *task1, *task2, *task4;
taskDescriptor *task3;
static int rotaryCount = 0;
static int joyCount = 0;

static int tens = 0;
static int sec = 0;

int main()
{
	//initializing the scheduler for Green LED toggle with 0.5s
	task1 = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	task1->task = &task_ledToggle;
	task1->period = 2000;
	task1->expire = task1->period;
	task1->execute = 0;
	task1->next = NULL;
	task1->param = NULL;
	task1->reserved = 0;
	scheduler_add(task1);

	//initializing task2 for stopwatch behavior
	task2 = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	task2->task = &task_stopWatch;
	task2->period = 100;
	task2->expire = task2->period;
	task2->execute = 0;
	task2->param = NULL;
	task2->reserved = 0;
	task2->next = NULL;
//	scheduler_add(task2);


	//initializing the scheduler for Joystick toggle
	task3 = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	task3->task = &call_yellowToggle;	//used extra function because of param receive
	task3->period  = 0;
	task3->expire = 5000;
	task3->execute = 1;
	task3->next = NULL;
	task3->param = NULL;
	task3->reserved = 0;
	//scheduler_add(task3);


	//initializing the task for button_checkState
	task4 = (taskDescriptor *)malloc(sizeof(taskDescriptor));
	task4->task = &button_checkState;
	task4->period = 5;
	task4->execute = 0;
	task4->expire = task4->period;
	task4->next = NULL;
	task4->param = NULL;
	task4->reserved = 0;
	scheduler_add(task4);

	button_init(true);	//using button debouncing feature
	led_greenInit();
	led_redInit();
	led_yellowInit();
	scheduler_init();
	lcd_init();
	uart_init(57600);

	led_greenOff();
	led_yellowOff();
	led_redOff();

	//setting callback pointers on pressing the buttons
	button_setRotaryButtonCallback(&initialize_aperiodicTask);
	button_setJoystickButtonCallback(&task_pressJoystick);

	sei();

	while(true)
		scheduler_run();

	return 0;
}


//To be called from scheduler every 2 seconds for green led toggle
void task_ledToggle(void *param)
{
	led_greenToggle();
}

//called on pressing joystick button, i.e joystick callback
void task_pressJoystick()
{

	joyCount++;
	//Detecting the second time press to execute once again and to remove task3 so updating the variables
	if(joyCount%2 == 0 && !scheduler_add(task3))
	{
		task3->execute = 1;
		task3->expire = task3->period;
		joyCount = 0;
	}
	//If the button is pressed again after 5 secs and for first time, adding the task
	else if(joyCount%2 == 1 && scheduler_add(task3))
		;	//Adding task for the first time
}

//called from scheduler to perform toggle for joystick press
void call_yellowToggle(void* param)
{
	led_yellowToggle();
}

//called when rotary button is pressed, i.e rotary callback
void initialize_aperiodicTask()
{
	if(rotaryCount == 0)
	{
		scheduler_add(task2);
	}
	rotaryCount++;
}

//called from scheduler to perform stopwatch task for rotary press
void task_stopWatch(void *param)
{
	//Detecting first time press and odd presses to start the timer
	if(rotaryCount%2 == 1 && rotaryCount > 0)
	{
		tens++;
		if(tens == 10)
		{
			sec++;
			tens = 0;
		}
		lcd_setCursor(0, 0);
		fprintf(lcdout, "TIMER STARTS %d %d", sec, tens);
	}
	//Detecting the second time press and even presses to stop the timer
	else if(rotaryCount%2 == 0 && rotaryCount > 0)
	{
		lcd_setCursor(0, 1);
		fprintf(lcdout, "TIMER STOPS %d %d", sec, tens);
	}
}
