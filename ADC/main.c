#include <avr/io.h>
#include <util/delay.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ses_common.h"
#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_uart.h"
#include "ses_button.h"
#include "ses_adc.h"


int main()
{
	//Initialization
	led_redInit();
	led_greenInit();
	led_yellowInit();
	button_init(true);
	led_redOff();
	led_greenOff();
	led_yellowOff();
	lcd_init();


	//initialize ADC
	adc_init();





	while(1)
	{
		uint16_t i,j,k;


		//Switch on the red led on pressing rotary button

		if(button_isRotaryPressed() == false)
			led_redOn();
		else
			led_redOff();

		//Switch on the green led on pressing joystick button

		 if(button_isJoystickPressed() == false)
				led_greenOn();
		 else
			 led_greenOff();

		 //Switch on the yellow led when joystick is in left position

		 if (adc_getJoystickDirection() == LEFT)
			 led_yellowOn();
		 else
			 led_yellowOff();

		 //Printing the milli seconds

		 unsigned long us = 16000;
		 while (us--){
			 asm volatile ( "nop" );
		 }
		 i++;
		 lcd_setCursor(0,0);
		 fprintf(lcdout, "SECONDS(ms) %4u", i);


		 //Printing the temprature

		 if (i%2500==0){
		 lcd_setCursor(0,1);
		 j++;
		 fprintf(lcdout, "COUNTER %4u", j);

		 lcd_setCursor(0,2);
		 int8_t temprature = adc_getTemperature()/10;				//Reading temprature value
		 fprintf(lcdout, "TEMPRATURE %4u", temprature);				//Printing Temprature

		 int8_t light = adc_read(ADC_LIGHT_CH);						//Reading Light value
		 lcd_setCursor(0,3);
		 fprintf(lcdout, "LIGHT %4u", light);						//Printing Temprature
		 }
	}
}
