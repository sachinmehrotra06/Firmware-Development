#ifndef SES_fsm_H_
#define SES_fsm_H_

/*INCLUDES-------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "ses_common.h"
#include "ses_scheduler.h"

/* TYPES ********************************************************************/
typedef struct fsm_s Fsm;		//< typedef for alarm clock state machine
typedef struct event_s Event;
typedef struct time_t time_t;

/** typedef for state event handler functions */
typedef uint8_t fsmReturnStatus; 		 //< typedef to be used with above enum
typedef fsmReturnStatus (*State)(Fsm *, const Event*);

/*Definition of Structure************************************************************/
struct time_t {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t milli;
};

struct fsm_s {
	State state;					//< current state, pointer to event handler
	bool isAlarmEnabled;			//< flag for the alarm status
	time_t timeSet;					//< multi-purpose var for system time and alarm time
};

struct event_s {
	uint8_t signal;					//< identifies the type of event
};

/*Enumration Declation *************************************************************/

/** return values */
enum {
	RET_HANDLED,    			//< event was handled
	RET_IGNORED,				//< event was ignored; not used in this implementation
	RET_TRANSITION 				//< event was handled and a state transition occurred
};

/** Possible Event */
enum {
	JOYSTICK_PRESSED,
	ROTARY_PRESSED,
	ENTRY,
	EXIT,
	MATCH,
	EXPIRY,
	CLOCKWISE,
	COUNTERCLOCK,
};

/*MACROS  ************************************************************/

#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)	//Used to transition from one state to another


/*Function Prototypes*****************************************************************/

void myfsm_init(taskDescriptor t1, taskDescriptor t2);
void timeDisplay();
systemTime_t timeToMilliseconds(time_t tm);
time_t timeFromScheduler();

fsmReturnStatus uninitializedClock(Fsm * fsm, const Event* event);
fsmReturnStatus setHours(Fsm * fsm, const Event* event);
fsmReturnStatus setMinutes(Fsm * fsm, const Event* event);
fsmReturnStatus runningTime(Fsm * fsm, const Event* event);
fsmReturnStatus alarmSetHours(Fsm * fsm, const Event* event);
fsmReturnStatus alarmSetMinutes(Fsm * fsm, const Event* event);
fsmReturnStatus alarmRunning(Fsm * fsm, const Event* event);
fsmReturnStatus alarmBuzzer(Fsm * fsm, const Event* event);

#endif /* fsm_H_ */
