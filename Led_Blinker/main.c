
#include <avr/io.h>
#include <util/delay.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ses_common.h"
#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_uart.h"

void print_state(const char *);

int main()
{
	//Initialization
			led_redInit();
			led_greenInit();
			led_yellowInit();
			uart_init(57600);
			lcd_init();
			fprintf(uartout, "START\n");
			fprintf(lcdout, "START");



	while(1)
	{


		//Printing on LCD



		//Traffic Light Working Model
		lcd_clear();
		led_greenOn();
		lcd_setCursor(0,0);
		print_state("GO");



		_delay_ms(5000);
		led_greenOff();
		lcd_clear();
		led_yellowOn();
		lcd_setCursor(0,0);
		print_state("WAIT");

		_delay_ms(500);
		led_yellowOff();
		led_redOn();
		lcd_clear();
		lcd_setCursor(0,0);
		print_state("STOP");

		_delay_ms(2000);
		led_redOff();
		led_yellowOn();
		lcd_clear();
		lcd_setCursor(0,0);
		print_state("WAIT");

		_delay_ms(300);
		led_yellowOff();

	}


}


void print_state(const char* state)
{
	//void lcd_init();
	fprintf(uartout, "%s\n", state);
	fprintf(lcdout, "%s\n", state);


}
