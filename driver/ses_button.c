#include "ses_common.h"
#include "ses_button.h"
#include "ses_led.h"


//Button wiring on SES board
#define BUTTON_PORTS		PORTB
#define ROTARY_BUTTON_PIN		6
#define JOY_BUTTON_PIN			7

//Macro
#define BUTTON_NUM_DEBOUNCE_CHECKS 5

//Function Pointers
static pButtonCallback cbRotary = NULL;
static pButtonCallback cbJoystick = NULL;

/*FUNCTION DEFINITIONS *****************/

void button_init(bool debouncing)
{
	DDR_REGISTER(BUTTON_PORTS) &= ~(1 << ROTARY_BUTTON_PIN);
	DDR_REGISTER(BUTTON_PORTS) &= ~(1 << JOY_BUTTON_PIN);

	BUTTON_PORTS |= 1 << ROTARY_BUTTON_PIN;
	BUTTON_PORTS |= 1 << JOY_BUTTON_PIN;

	//Activating pin change interrupt between PCINT7 to 0
//	PCICR |= 1 << PCIE0;

	//Enabling the interrupt pins in PCMSK0 for PCINT7:0 interrupts
//	PCMSK0 |= 1 << ROTARY_BUTTON_PIN;
//	PCMSK0 |= 1 << JOY_BUTTON_PIN;

	if(debouncing)
	{
		timer1_setCallback(&button_checkState);
		//deactivating pin change interrupt
		PCICR &= ~(1 << PCIE0);
		//initialization for debouncing
		timer1_start();
	}
	else
	{
		//For direct interrupt changes
		PCICR |= 1 << PCIE0;
		PCMSK0 |= 1 << ROTARY_BUTTON_PIN;
		PCMSK0 |= 1 << JOY_BUTTON_PIN;
	}
}


bool button_isJoystickPressed()
{
	bool val = false;
	if(!(PIN_REGISTER(BUTTON_PORTS) & (1 << JOY_BUTTON_PIN)))
		val = true;

	return val;
}


bool button_isRotaryPressed()
{
	bool val = false;
	if(!(PIN_REGISTER(BUTTON_PORTS) & (1 << ROTARY_BUTTON_PIN)))
		val = true;

	return val;
}

//Implementing the inbuilt Interrupt Service Routine
ISR(PCINT0_vect)
{
	if(button_isJoystickPressed())
	{
		if(cbJoystick != NULL && (PCMSK0 & 1 << JOY_BUTTON_PIN))
			cbJoystick();
	}else if(button_isRotaryPressed())
	{
		if(cbRotary != NULL && (PCMSK0 & 1 << ROTARY_BUTTON_PIN))
			cbRotary();
	}
}


//implementing the setter function for Rotary and Joystick buttons
void button_setRotaryButtonCallback(pButtonCallback callback)
{
	cbRotary = callback;
}

void button_setJoystickButtonCallback(pButtonCallback callback)
{
	cbJoystick = callback;
}

void button_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	uint8_t change = 0x00;

	// each bit in every state byte represents one button
	state[index] = 0;
	if(button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if(button_isRotaryPressed()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}

	// init compare value and compare with ALL reads, only if
	// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "0" in the state
	// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for(uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;
	change = debouncedState ^ lastDebouncedState;
	if((change & 1) && button_isJoystickPressed() && cbJoystick != NULL)
	{
		cbJoystick();
	}
	if((change & 2) && button_isRotaryPressed() && cbRotary != NULL)
	{
		cbRotary();
	}
}
