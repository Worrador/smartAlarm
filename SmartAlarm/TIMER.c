/*
 * PWM.c
 *
 * Created: 2020-02-18 15:39:56
 *  Author: Ákos
 */ 
#include <avr/io.h>
#include "TIMER.h"
#include <stdint.h> 
#include "LCD.h"
#include "main.h"
#include <avr/interrupt.h>
#include "BUTTONS.h"
#include <stdbool.h>

uint16_t secondsCntr = 0;

void init_pwm_timer()
{
	// 8 bit timer
	TCCR0A |= (1 << COM0A1);				// set none-inverting mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00);	// set fast PWM Mode
	TCCR0B = (1 << CS01);					// clock = clk/8 -> 16MHz/8 = 2MHz -> 2MHz/256 = 7.81kHz pwm frequency
	OCR0A = 0;								// Compare value is BOTTOM
	PRR |= (1 << PRTIM0);					// Disable timer on startup
}

void config_pwm_timer(uint8_t compareValue)
{
	OCR0A  =  compareValue;					// 50% 8 bit number to which it will compare the counter value 2^8 = 256
}



void init_oneSecond_timer()
{
    OCR1A = 0xB71A;//0x3D08;					// 3 sec timer interrupts :)

    TCCR1B |= (1 << WGM12);						// Mode 4, CTC on OCR1A
    TIMSK1 |= (1 << OCIE1A);					// Set interrupt on compare match
    TCCR1B |= (1 << CS12) | (1 << CS10);	    // set prescaler to 1024 and start the timer
}

ISR (TIMER1_COMPA_vect)
{
	secondsCntr+=3;
	if(secondsCntr >= 60)
	{
		secondsCntr = 0;
		new_currentMinutes++;
		
		if(new_currentMinutes >= 60)
		{
			new_currentMinutes = 0;
			new_currentHours++;
			
			if(new_currentHours >= 24)
			{
				new_currentHours = 0;
				refreshAlarm = true;
				send_UART("refreshalarm was set to true\n\r");
				get_RTC_time();
				
				/*if(new_currentDay != sun)
				{
					new_currentDay++;
				}
				else
				{
					new_currentDay = mon;
				}*/
			}else if(new_currentHours == 23)
			{
				LED &= ~(1 << 3);
			}else if(new_currentHours == 10)
			{
				LED |= (1 << 3);
			}
		}
		
		// This cant be in the statemachine because events would be conflicting sometimes
		if(SET_ALARM != STATE)
		{
			update_LCD(new_currentHours, new_currentMinutes);
		}
	}

	if(startUp == 0)
	{
		update_LCD(new_currentHours, new_currentMinutes);
		startUp = 2;
	}
	new_currentTime = (new_currentHours << 8) + new_currentMinutes;
	/* This implements the toggling of the column:
	if(SET_ALARM != STATE)
	{
 		if(secondsCntr % 2 == 0)
 		{
	 		write_LCD_str(":", (LINE1+0x08));
 		}
 		else
 		{
	 		write_LCD_str(" ", (LINE1+0x08));
 		}
 	}*/


	// This cant be in the statemachine or else it would need a delay, which block its operation
	if((WAKING_UP == STATE) && (secondsCntr % 2 == 0))		// update brightness every 6 seconds
	{
		update_brigthness();
	}

}
