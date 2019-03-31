/* INCLUDES ******************************************************************/

#include "ses_pwm.h"

/* DEFINES & MACROS **********************************************************/

#define MOTOR_PORT	PORTG
#define MOTOR_PIN	5

/* FUNCTION DEFINITION *******************************************************/

void pwm_init()
{
	//Enabling the timer 0
	PRR0 &= ~(1 << PRTIM0);

	//Registering PORTG 5 pin for output to MOTOR from controller
	DDR_REGISTER(MOTOR_PORT) |= 1 << MOTOR_PIN;

	//selecting the timer/counter0 in Fast PWM mode	with TOP as 0xFF default val
	TCCR0A |= 1 << WGM01;
	TCCR0A |= 1 << WGM00;
	TCCR0B &= ~(1 << WGM02);

	//Disabling the prescaler val for timer0
	TCCR0B &= ~(1 << CS02);
	TCCR0B &= ~(1 << CS01);
	TCCR0B |= 1 << CS00;

	//Configuring OC0B to set on compare match
	TCCR0A |= 1 << COM0B1;
//	TCCR0A |= 1 << COM0B0;	//non inverting mode and "keeping this dead code to verify both modes later"
	TCCR0A &= ~(1 << COM0B0);	//zero for inverting mode
}

//To set the duty cycle value to run motor
void pwm_setDutyCycle(uint8_t dutyCycle)
{
	OCR0B = dutyCycle;
}
