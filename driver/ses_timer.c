#include "ses_timer.h"

#define VAL_1MILISECOND 249
#define VAL_5MILISECONDS 1249

static pTimerCallback cbTimer2 = NULL;
static pTimerCallback cbTimer1 = NULL;

static pTypeRotaryCallback cbTimer1_2 = NULL;


//Setting a function to be called when timer fires
void timer2_setCallback(pTimerCallback cb)
{
	cbTimer2 = cb;
}



void timer2_start()
{
	//clear timer on compare match
	TCCR2A |= 1 << WGM21;
	TCCR2A &= ~(1 << WGM20);

	//selecting prescaler of 64
	//TCCR2B &= ~(1 << FOC2A)
	TCCR2B &= ~(1 << CS20);
	TCCR2B &= ~(1 << CS21);
	TCCR2B |= (1 << CS22);

	//Set interrupt mask register for compare A
	TIMSK2 |= 1 << OCIE2A;

	//clear an interrupt flag by setting 1 in register for compare A
	TIFR2 &= ~(1 << OCF2A);

	//generate interrupt every 1 ms
	OCR2A = VAL_1MILISECOND;
}

void timer2_stop()
{
	TCCR2B &= ~(1 << CS20);
	TCCR2B &= ~(1 << CS21);
	TCCR2B &= ~(1 << CS22);
}

//ISR to calling the task on interrupt
ISR(TIMER2_COMPA_vect)
{
	cbTimer2();
}

void timer1_setCallback(pTimerCallback cb)
{
	cbTimer1 = cb;
}

void timer1_setCallback2(pTypeRotaryCallback cb)
{
	cbTimer1_2 = cb;
}

void timer1_start()
{
	//Clear Timer on Compare Match
	TCCR1B |= 1 << WGM12;

	//Select a prescaler of 64
	TCCR1B |= 1 << CS10 ;
	TCCR1B |= 1 << CS11;

	//Set interrupt mask register for Compare A
	TIMSK1 |= 1 << OCIE1A;

	//Clear the interrupt flag by setting an 1 in register for Compare A
	TIFR1 |= 1 << OCF1A;

	//Set a value in register OCR2A in order to generate an interrupt every 5 ms
	OCR1A = VAL_5MILISECONDS;
}

void timer1_stop()
{
	TCCR1B &= ~(1 << WGM12);
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
}

//ISR to calling the task on timer1 interrupt
ISR(TIMER1_COMPA_vect)
{
	cbTimer1();
	cbTimer1_2();
}
