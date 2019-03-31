/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_adc.h"


/* DEFINES & MACROS **********************************************************/

//adc wiring on SES board
#define TEMPRATURE_SENSOR_PORT                 PORTF
#define TEMPRATURE_SENSOR_PIN					2

#define LIGHT_SENSOR_PORT                 		PORTF
#define LIGHT_SENSOR_PIN						4

#define JOYSTICK_PORT                 	PORTF
#define JOYSTICK_PIN						5

#define MICROPHONE_PORT                 	PORTF
#define MICROPHONE_PIN_A					0

#define MICROPHONE_PORT                	PORTF
#define MICROPHONE_PIN_B					1

#define JOYSTICK_RIGHT					200
#define JOYSTICK_UP						400
#define JOYSTICK_LEFT					600
#define JOYSTICK_DOWN					800
#define JOYSTICK_NO_DIRECTION			1000
#define TOLERANT						50

#define ADC_TEMP_RAW_MAX				481
#define ADC_TEMP_RAW_MIN				256

#define ADC_TEMP_FACTOR					10
#define ADC_TEMP_MAX					400*ADC_TEMP_FACTOR
#define ADC_TEMP_MIN					200*ADC_TEMP_FACTOR

/* FUNCTION DEFINITION *******************************************************/

void adc_init(void){

	//Configure the data direction registers (temperature, light, microphone, joystick)
	DDR_REGISTER(TEMPRATURE_SENSOR_PORT) &= ~(1 << TEMPRATURE_SENSOR_PIN);
	DDR_REGISTER(JOYSTICK_PORT) &= ~(1 << JOYSTICK_PIN);
	DDR_REGISTER(LIGHT_SENSOR_PORT) &= ~(1 << LIGHT_SENSOR_PIN);


	//deactivate the internal pull-up resistors.
	TEMPRATURE_SENSOR_PORT &= ~( 1 << TEMPRATURE_SENSOR_PIN);
	JOYSTICK_PORT &= ~( 1 << JOYSTICK_PIN);
	LIGHT_SENSOR_PORT &= ~( 1 << LIGHT_SENSOR_PIN);

	//Disable Power Reduction Mode
	PRR0 &= ~( 1 << PRADC);

	//Configure voltage reference to 1.6v
	ADMUX |= 1 << REFS1;
	ADMUX |= 1 << REFS0;

	//clear ADALAR Bit
	ADMUX &= ~(1 << ADLAR);

	ADCSRB &= ~(1 << MUX5);

	//Configure ADC clock
	ADCSRA |= ADC_PRESCALE;

	//Not selecting auto triggering ( ADATE )
	ADCSRA &= ~(1 << ADATE);

	//Enable the ADC
	ADCSRA |= 1<< ADEN;

}

//Reading the ADC channel

uint16_t adc_read(uint8_t adc_channel){

	if (adc_channel >= ADC_NUM){
		return ADC_INVALID_CHANNEL;
	}
	else{
		ADMUX &= ~(1 << MUX0 | 1<< MUX1 | 1 << MUX2 | 1 << MUX3 | 1 << MUX4);
		ADMUX |= adc_channel;
		ADCSRA |= 1 << ADSC;
		while (ADCSRA & (1 << ADSC)){

		}
		return ADC;

	}
}

//REturning the Joystick direction after reading the RAW value

uint8_t adc_getJoystickDirection(void){
	uint16_t joystick_Value;

	joystick_Value = adc_read(ADC_JOYSTICK_CH);
	if (joystick_Value >= JOYSTICK_RIGHT - TOLERANT && joystick_Value <= JOYSTICK_RIGHT + TOLERANT){
		return RIGHT;
	}
	else if (joystick_Value >= JOYSTICK_UP - TOLERANT && joystick_Value <= JOYSTICK_UP + TOLERANT){
		return UP;
	}
	else if (joystick_Value >= JOYSTICK_DOWN - TOLERANT && joystick_Value <= JOYSTICK_DOWN + TOLERANT){
		return DOWN;
	}
	else if (joystick_Value >= JOYSTICK_LEFT - TOLERANT && joystick_Value <= JOYSTICK_LEFT + TOLERANT){
		return LEFT;
	}
	else
		return NO_DIRECTION;

}

//Calculating the temprature in degree celcius from RAW ADC output

int16_t adc_getTemperature(void){

	int16_t adc = adc_read(ADC_TEMP_CH);
	int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
	return (adc * slope + offset) / ADC_TEMP_FACTOR;
}
