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
#include "ses_timer.h"

void softwareTimer2(void){

	static i=0,t=0;
	i++;
	if (i==1000){
		led_yellowToggle();
		i=0;
		t++;
		lcd_setCursor(0,0);
		fprintf(lcdout, "TIME2 %4u", t);

	}

}

void softwareTimer1(void){

	static j=0,t2=0;
	j++;
	if (j==200){
		led_redToggle();
		j=0;
		t2++;
		lcd_setCursor(0,1);
		fprintf(lcdout, "TIME1 %4u", t2);

	}


}

int main()
{
	//Initialization
	led_redInit();
	led_greenInit();
	led_yellowInit();
	button_init(true);

	led_redOff();
	led_greenOff();
	lcd_init();
	timer2_start();
	led_yellowOff();
	uart_init(57600);
	led_redInit();
	timer1_start();

	button_setRotaryButtonCallback(led_greenToggle);
	button_setJoystickButtonCallback(led_redToggle);
	timer2_setCallback(softwareTimer2);
	//timer1_setCallback(softwareTimer1);

	sei();

	while(1)
	{
	}


	return 0;
}
