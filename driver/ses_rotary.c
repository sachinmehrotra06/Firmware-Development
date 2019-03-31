/*INCLUDES ************************************************************/

#include "ses_rotary.h"
#include "ses_timer.h"
#include "ses_common.h"
#include "ses_lcd.h"

/* TYPES AND MACROS *******************************************************/
// Rotary wiring on SES board
#define ROTARY_INPUTS_A_PORT      PORTB
#define ROTARY_INPUTS_A_PIN			5

#define ROTARY_INPUTS_B_PORT		PORTG
#define ROTARY_INPUTS_B_PIN			2
#define BUTTON_NUM_DEBOUNCE_CHECKS 3

/* GLOBAL VARIABLES **************************************************/
static pTypeRotaryCallback cbClockwise = NULL;
static pTypeRotaryCallback cbCounterclockwise = NULL;
static bool sampling = false;
bool a = true;
bool b = true;


/*FUNCTION DEFINATION ************************************************/

void rotary_init(){
	DDR_REGISTER(ROTARY_INPUTS_A_PORT) &= ~(1 << ROTARY_INPUTS_A_PIN);
	DDR_REGISTER(ROTARY_INPUTS_B_PORT) &= ~(1 << ROTARY_INPUTS_B_PIN);

	ROTARY_INPUTS_A_PORT |= 1 << ROTARY_INPUTS_A_PIN;
	ROTARY_INPUTS_B_PORT |= 1 << ROTARY_INPUTS_B_PIN;

	/* initialization for rotation debouncing and timer1 has started in button init function already
	 */
	timer1_setCallback2(&rotary_checkState);
}

bool rotary_isClockwise(void) {
	bool val;
	a = !(PIN_REGISTER(ROTARY_INPUTS_A_PORT) & (1 << ROTARY_INPUTS_A_PIN));
	b = !(PIN_REGISTER(ROTARY_INPUTS_B_PORT) & (1 << ROTARY_INPUTS_B_PIN));
	if(a!=b)
		sampling = true;
	val = a && sampling && !b;
	return val;

}

bool rotary_isCounterClockwise(void) {
	bool val;
	a = !(PIN_REGISTER(ROTARY_INPUTS_A_PORT) & (1 << ROTARY_INPUTS_A_PIN));
	b = !(PIN_REGISTER(ROTARY_INPUTS_B_PORT) & (1 << ROTARY_INPUTS_B_PIN));
	if(a!=b)
		sampling = true;
	val = b && sampling && !a;
	return val;

}

void rotary_setClockwiseCallback(pTypeRotaryCallback callback){
	cbClockwise = callback;
}

void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback){
	cbCounterclockwise = callback;
}


void rotary_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	uint8_t changed=0x00;

	state[index] = 0;
	if (rotary_isClockwise()) {
		state[index] |= 1;
	}
	if (rotary_isCounterClockwise()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}

	uint8_t j = 0xFF;
	for(uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}

	debouncedState = j;

	changed = debouncedState ^ lastDebouncedState;
	if ((changed & 1) && rotary_isClockwise() && cbClockwise != NULL){
		cbClockwise();
	}
	if ((changed & 2) && rotary_isCounterClockwise() && cbCounterclockwise != NULL){
		cbCounterclockwise();
	}

}
