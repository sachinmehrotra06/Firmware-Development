/*INCLUDES ************************************************************/

#include "ses_led.h"
#include "ses_common.h"
#include "ses_scheduler.h"
#include "ses_pwm.h"
#include "ses_button.h"
#include "ses_motorFrequency.h"
#include "ses_lcd.h"
#include "ses_scheduler.h"
#include "ses_fsm.h"
#include "ses_rotary.h"

/* GLOBAL VARIABLES **************************************************/
Fsm FSM;

bool alarmFlag = false;
static int fiveSec = 0;
time_t alarmTime;

taskDescriptor runningClock;
taskDescriptor alarm_blink;

/*STATIC DISPATCH FUNCTIONS *************************************************/

/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm* fsm, const Event* event) {
	static Event entryEvent = {.signal = ENTRY};
	static Event exitEvent = {.signal = EXIT};
	State s = fsm->state;
	fsmReturnStatus r = fsm->state(fsm, event);
	if (r==RET_TRANSITION) {
		s(fsm, &exitEvent);						//< call exit action of last state
		fsm->state(fsm, &entryEvent); 			//< call entry action of new state
	}
}


/*Rotary dispatch function */
static void rotaryPressedDispatch(void * param){
	Event e = {.signal = ROTARY_PRESSED};
	fsm_dispatch(&FSM, &e);
}

/*Joystick dispatch function */
static void joystickPressedDispatch(void * param){
	Event e = {.signal = JOYSTICK_PRESSED};
	fsm_dispatch(&FSM, &e);
}

/*Clockwise rotary dispatch function */
static void rotaryClockwiseDispatch(void * param){
	Event e = {.signal = CLOCKWISE};
	fsm_dispatch(&FSM, &e);
}

/*CounterClockwise rotary dispatch function */
static void rotaryCounterClockDispatch(void * param){
	Event e = {.signal = COUNTERCLOCK};
	fsm_dispatch(&FSM, &e);
}

/*When the actual time becomes equal to alarm time this trigger an event*/
static void matchDispatch()
{
	Event e = {.signal = MATCH};
	fsm_dispatch(&FSM, &e);
}

/*To trigger an event after 5 seconds when alarm starts buzzing */
static void expiryDispatch()
{
	Event e = {.signal = EXPIRY};
	fsm_dispatch(&FSM, &e);
}

/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm* fsm, State init) {
	Event entryEvent = {.signal = ENTRY};
	fsm->state = init;
	fsm->state(fsm, &entryEvent);

	button_setRotaryButtonCallback(rotaryPressedDispatch);
	button_setJoystickButtonCallback(joystickPressedDispatch);
	rotary_setClockwiseCallback(rotaryClockwiseDispatch);
	rotary_setCounterClockwiseCallback(rotaryCounterClockDispatch);
}


/*It is called from main.c to initialize scheduler tasks and FSM as well */
void myfsm_init(taskDescriptor t1, taskDescriptor t2){

	runningClock = t1;
	alarm_blink = t2;

	runningClock.task = &timeDisplay;
	alarm_blink.task = &led_redToggle;

	State init = &uninitializedClock;
	fsm_init(&FSM, init);
}

/*To convert time_t variable to uint32_t type*/
systemTime_t timeToMilliseconds(time_t tm)
{
	systemTime_t timeMillisecond;

	timeMillisecond = tm.hour * 3600000 + tm.minute * 60000 + tm.second * 1000;
	return timeMillisecond;

}

/*To get time from scheduler and convert it to time_t type*/
time_t timeFromScheduler()
{
	systemTime_t timesecond = 0;
	time_t tm;

	timesecond = scheduler_getTime()/1000;
	tm.hour = timesecond / 3600;
	timesecond = timesecond % 3600;
	tm.minute = timesecond / 60;
	tm.second = timesecond % 60;

	return tm;
}

/*Scheduler task to display time every one second*/
void timeDisplay(){

	time_t tm = timeFromScheduler();

	FSM.timeSet.hour = tm.hour;
	FSM.timeSet.minute = tm.minute;
	FSM.timeSet.second = tm.second;
	lcd_setCursor(0,0);
	fprintf(lcdout,"%2u:%2u:%2u", FSM.timeSet.hour, FSM.timeSet.minute, FSM.timeSet.second);

	led_greenToggle();

	//for alarm buzzer for 5 seconds
	if(fiveSec >= 1 && fiveSec <= 5)
	{
		if(--fiveSec == 0)
			expiryDispatch();
	}

	//for transition to alarmBuzzer if alarm matches
	if(FSM.isAlarmEnabled == true)
	{
		if(tm.hour == alarmTime.hour && tm.minute == alarmTime.minute)
			matchDispatch();
	}
}


/*STATE FUNCTION DEFINITIONS ****************************************************/

/*The first uninitialized state of time */
fsmReturnStatus uninitializedClock(Fsm * fsm, const Event* event){
	switch(event->signal){

	case ROTARY_PRESSED:
		return TRANSITION(setHours);

	case ENTRY:
		led_greenOff();
		fsm->isAlarmEnabled = false;
		lcd_setCursor(0,0);
		fprintf(lcdout,"HH:MM");
		lcd_setCursor(0,1);
		fprintf(lcdout,"SET TIME");
		return RET_HANDLED;

	default:
		return RET_IGNORED;
	}
}

/*State for setting the hours in fsm timeSet*/
fsmReturnStatus setHours(Fsm * fsm, const Event* event){

	switch(event->signal){

	case ROTARY_PRESSED:
		fsm->timeSet.hour++;
		fsm->timeSet.hour = fsm->timeSet.hour % 24;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u", fsm->timeSet.hour);
		return RET_HANDLED;

	case CLOCKWISE:
		fsm->timeSet.hour++;
		fsm->timeSet.hour = fsm->timeSet.hour % 24;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u", fsm->timeSet.hour);
		return RET_HANDLED;

	case COUNTERCLOCK:
		if(fsm->timeSet.hour > 0)
			fsm->timeSet.hour--;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u", fsm->timeSet.hour);
		return RET_HANDLED;

	case JOYSTICK_PRESSED:
		return TRANSITION(setMinutes);

	case ENTRY:
		lcd_clear();
		lcd_setCursor(0, 0);
		fprintf(lcdout, "00:00");
		lcd_setCursor(0,1);
		fprintf(lcdout,"SET HOURS");
		return RET_HANDLED;

	default:
		return RET_IGNORED;
	}
}

/*State for setting the minutes in fsm timeSet */
fsmReturnStatus setMinutes(Fsm * fsm, const Event* event){

	switch(event->signal){

	case ROTARY_PRESSED:
		fsm->timeSet.minute++;
		fsm->timeSet.minute = fsm->timeSet.minute % 60;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u:%2u",fsm->timeSet.hour, fsm->timeSet.minute);
		return RET_HANDLED;

	case CLOCKWISE:
		fsm->timeSet.minute++;
		fsm->timeSet.minute = fsm->timeSet.minute % 60;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u:%2u",fsm->timeSet.hour, fsm->timeSet.minute);
		return RET_HANDLED;

	case COUNTERCLOCK:
		if(fsm->timeSet.minute > 0)
			fsm->timeSet.minute--;
		lcd_setCursor(0,0);
		fprintf(lcdout,"%2u:%2u",fsm->timeSet.hour, fsm->timeSet.minute);
		return RET_HANDLED;

	case JOYSTICK_PRESSED:
		return TRANSITION(runningTime);

	case ENTRY:
		lcd_clear();
		lcd_setCursor(0, 0);
		fprintf(lcdout, "%u:00", fsm->timeSet.hour);
		lcd_setCursor(0,1);
		fprintf(lcdout,"SET MINUTE");
		return RET_HANDLED;

	default:
		return RET_IGNORED;

	}

}

/*Time running state to update time in scheduler variable from fsm timeSet */
fsmReturnStatus runningTime(Fsm * fsm, const Event* event){
	systemTime_t timeMillisecond;

	switch(event->signal){
	case ROTARY_PRESSED:
		if(fsm->isAlarmEnabled != true)
		{
			fsm->isAlarmEnabled = true;
			led_yellowOn();
		}
		else
		{
			fsm->isAlarmEnabled = false;
			led_yellowOff();
		}
		return TRANSITION(alarmRunning);

	case JOYSTICK_PRESSED:
		return TRANSITION(alarmSetHours);

	case ENTRY:
		led_redOff();
		timeMillisecond = timeToMilliseconds(fsm->timeSet);
		scheduler_setTime(timeMillisecond);
		scheduler_add(&runningClock);

		lcd_clear();
		lcd_setCursor(0, 1);
		fprintf(lcdout, "ROTARY-ALARM ON/OFF");
		lcd_setCursor(0, 2);
		fprintf(lcdout, "JOYSTCK -SET ALARM");
		return RET_HANDLED;

	case EXIT:
		scheduler_remove(&runningClock);
		return RET_IGNORED;

	default:
		return RET_IGNORED;

	}
}

/*State for setting the hours in alarm structure */
fsmReturnStatus alarmSetHours(Fsm * fsm, const Event* event){
	switch(event->signal){

	case ROTARY_PRESSED:
		alarmTime.hour++;
		alarmTime.hour = alarmTime.hour % 24;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u", alarmTime.hour);
		return RET_HANDLED;

	case CLOCKWISE:
		alarmTime.hour++;
		alarmTime.hour = alarmTime.hour % 24;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u", alarmTime.hour);
		return RET_HANDLED;

	case COUNTERCLOCK:
		if(alarmTime.hour > 0)
			alarmTime.hour--;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u", alarmTime.hour);
		return RET_HANDLED;

	case JOYSTICK_PRESSED:
		return TRANSITION(alarmSetMinutes);

	case ENTRY:
		lcd_clear();
		lcd_setCursor(0, 1);
		fprintf(lcdout, "00:00");
		scheduler_add(&runningClock);
		lcd_setCursor(0,2);
		fprintf(lcdout,"SET ALARM HRS");
		alarmTime.hour = 0;
		return RET_HANDLED;

	default:
		return RET_IGNORED;
	}
}

/*State for setting the minutes in alarm structure variable*/
fsmReturnStatus alarmSetMinutes(Fsm * fsm, const Event* event){
	switch(event->signal){

	case ROTARY_PRESSED:
		alarmTime.minute++;
		alarmTime.minute = alarmTime.minute % 60;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u:%2u",alarmTime.hour, alarmTime.minute);
		return RET_HANDLED;

	case CLOCKWISE:
		alarmTime.minute++;
		alarmTime.minute = alarmTime.minute % 60;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u:%2u",alarmTime.hour, alarmTime.minute);
		return RET_HANDLED;

	case COUNTERCLOCK:
		if(alarmTime.minute > 0)
			alarmTime.minute--;
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u:%2u",alarmTime.hour, alarmTime.minute);
		return RET_HANDLED;


	case JOYSTICK_PRESSED:
		return TRANSITION(alarmRunning);

	case ENTRY:
		lcd_setCursor(0, 1);
		fprintf(lcdout, "%2u:00", alarmTime.hour);
		lcd_setCursor(0,2);
		fprintf(lcdout,"SET ALARM MNTS");
		alarmTime.minute = 0;
		return RET_HANDLED;

	case EXIT:
		return RET_IGNORED;

	default:
		return RET_IGNORED;
	}
}


/*Alarm is set and time is running, takes transition to alarmBuzzer state when gets MATCH event*/
fsmReturnStatus alarmRunning(Fsm * fsm, const Event* event){

	switch(event->signal){

	case ROTARY_PRESSED:
		if(fsm->isAlarmEnabled != true)
		{
			fsm->isAlarmEnabled = true;
			led_yellowOn();
			return RET_HANDLED;
		}
		else
		{
			fsm->isAlarmEnabled = false;
			led_yellowOff();
			return TRANSITION(runningTime);
		}

	case JOYSTICK_PRESSED:
		scheduler_remove(&runningClock);
		return TRANSITION(alarmSetHours);

	case ENTRY:
		lcd_clear();
		scheduler_add(&runningClock);
		lcd_setCursor(0,1);
		fprintf(lcdout,"%2u:%2u", alarmTime.hour,alarmTime.minute);

		lcd_setCursor(0, 2);
		fprintf(lcdout, "ROTARY -ALARM ON/OFF");
		lcd_setCursor(0, 3);
		fprintf(lcdout, "JOYSTCK -SET ALARM");
		return RET_HANDLED;

	case EXIT:
		return RET_HANDLED;

	case MATCH:
		lcd_clear();
		lcd_setCursor(0,1);
		fprintf(lcdout,"Alarm time matched!");
		return TRANSITION(alarmBuzzer);

	default:
		return RET_IGNORED;
	}
}

/*Alarm is matched to actual time and alarm will be disabled on exit of this state */
fsmReturnStatus alarmBuzzer(Fsm * fsm, const Event* event)
{

	switch(event->signal){

	case ENTRY:
		scheduler_add(&alarm_blink);
		fiveSec = 5;
		lcd_clear();
		lcd_setCursor(0,2);
		fprintf(lcdout,"Any Button to Stop!");
		return RET_HANDLED;

	case ROTARY_PRESSED:
		return TRANSITION(runningTime);

	case JOYSTICK_PRESSED:
		return TRANSITION(runningTime);

	case EXPIRY:
		return TRANSITION(runningTime);

	case EXIT:
		scheduler_remove(&alarm_blink);
		scheduler_remove(&runningClock);
		fsm->isAlarmEnabled = false;
		led_yellowOff();
		return RET_HANDLED;

	default:
		return RET_IGNORED;
	}
}
