/* INCLUDES ******************************************************************/

#include "ses_motorFrequency.h"

/* DEFINES & MACROS **********************************************************/

#define EXT_INT_PORT	PORTD
#define EXT_INT_PIN		0

#define TIMER_CYCLE_100MILISEC	24999
#define INTERVALS	101		//N Median values

/* GLOBAL VARIABLES & ARRAYS **********************************************************/

static int interrupt_count = 0;
static uint16_t retVal, retVal2, time5Val = 0;

uint16_t intervals[INTERVALS] = {0};

int n, j, swap;
static int i = 0;

/* FUNCTION DEFINITION *******************************************************/

void motorFrequency_init()
{
	//Enabling the port D to read spike signal
	DDR_REGISTER(EXT_INT_PORT)	&= ~(1 << EXT_INT_PIN);

	//Enabling the external timer at pin 0
	EIMSK |= 1 << INT0;

	//An edge occured on pin INT0 has triggered an interrupt
	EIFR |= 1 << INTF0;

	//Taking rising edge of INTn generates an interrupt asynchronously
	EICRA |= 1 << ISC00;
	EICRA |= 1 << ISC01;

	//Timer 5 configuration
	//Clear Timer on Compare Match
	TCCR5B |= 1 << WGM52;

	//Select a prescaler of 64
	TCCR5B |= 1 << CS50 ;
	TCCR5B |= 1 << CS51;

	//Set interrupt mask register for Compare A
	TIMSK5 |= 1 << OCIE5A;

	//Clear the interrupt flag by setting an 1 in register for Compare A
	TIFR5 |= 1 << OCF5A;

	//Set a value in register OCR2A in order to generate an interrupt every 100ms
	OCR5A = TIMER_CYCLE_100MILISEC;

}

//Interrupt service routine for external interrupt at INT0 pin
//To record the timer5 value on every one rotation
ISR(INT0_vect)
{
	led_yellowToggle();

	interrupt_count++;
	if(interrupt_count == 6)
	{
		time5Val = TCNT5;
		TCNT5 = 0;
		interrupt_count = 0;

		//Storing the N values of timer5 in array to calculate Median value later
		if(i != INTERVALS)
		{
			intervals[i] = time5Val;
			i++;
		}
		else
			i = 0;
	}
}

//Timer5 ISR to detect that the motor has stopped
ISR(TIMER5_COMPA_vect)
{
	led_greenOn();

	time5Val = 0;

	//Resetting the Median values to 0 to have RPM as 0
	for(n = 0; n < INTERVALS - 1; n++)
		intervals[n] = 0;
}

//Calculating the frequency of motor and returning it every second
uint16_t motorFrequency_getRecent()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		if(time5Val == 0)
			retVal2 = 0;
		else
			retVal2 = 250000/time5Val;
	}

	return retVal2;
}

//To return RPM value in Hertz
uint16_t motorFrequency_getMedian()
{
	uint16_t medVal = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		//Sorting the values of intervals in ascending order
		for(n = 0; n < INTERVALS - 1; n++)
		{
			for(j = 0; j < INTERVALS - i - 1; j++)
			{
				if(intervals[j] > intervals[j+1])
				{
					swap = intervals[j+1];
					intervals[j+1] = intervals[j];
					intervals[j] = swap;
				}
			}
		}
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		//Fetching the Median value and returning in Hertz
		if(INTERVALS % 2 == 0)
			medVal = intervals[INTERVALS/2];
		else
			medVal = intervals[INTERVALS/2 + 1];

		retVal = 1 + 1000000 / (medVal * 4);	//returning Hertz
	}

	return retVal;
}
