/*
 * GPIO.c
 *
 * Created: 2020-02-18 15:02:15
 *  Author: Ákos
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "GPIO.h"
#include <avr/interrupt.h>
#include "UART.h"
#include "RTC.h"
#include "main.h"
#include "TIMER.h"
#include "LCD.h"
#include "BUTTONS.h"
#include <stdbool.h>

uint8_t interruptCntr = 0;

void config_gpio(){
	// operation led:
	DDRB |= (1 << DDB5);	// pin B5 is output
	
	// pwm output:
	DDRB |= (1 << DDB2);	// pin B2 as pwm output
	
	// alarm setting buttons: hour, quarter hour, minute and then there is the set alarm button:
	DDRB &= ~(1 << DDB0 | 1 << DDB1 | 1 << DDB3 | 1 << DDB4 | 1 << DDB7);			// pins B0-1-3-4 and B7 are inputs
	PORTB |= (1<<PORTB0 | 1<<PORTB1 | 1<<PORTB3 | 1<<PORTB4 | 1<<PORTB7);			// Turn on the pull-up resistors
	
	// RTC interrupt pin 
    DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input

    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled

    EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0);     // Turns on INT0
}



void idle_led_toggle(){
	PINB |= (1 << PINB5);
	_delay_ms(1000);
}

ISR (INT0_vect)		// Alarm interrupt, 1 minute has passed
{
	if(interruptCntr == 2)
	{
		new_currentMinutes++;
				
		if(new_currentMinutes >= 60)
		{
			new_currentMinutes = 0;
			new_currentHours++;
					
			if(new_currentHours >= 24)
			{
				new_currentHours = 0;
				refreshAlarm = true;
			}else if(new_currentHours == 23)
			{
				LED &= ~(1 << 3);
			}else if(new_currentHours == 10)
			{
				LED |= (1 << 3);
			}else if(new_currentHours == 12)
			{
				get_RTC_time();		// Update time at noon to compensate for long-term inaccuracies
			}
		}
				
		// This cant be in the statemachine because events would be conflicting sometimes
		if(SET_ALARM != STATE)
		{
			update_LCD(new_currentHours, new_currentMinutes);
		}

		new_currentTime = (new_currentHours << 8) + new_currentMinutes;

		// This cant be in the statemachine or else it would need a delay, which block its operation
		if(WAKING_UP == STATE)
		{
			update_brigthness();
		}
		
		// If the read out time is the same as the current one, then start the wake-up sequence:
		if((new_alarmTime & 0x7F) == new_currentTime)
		{
			if((new_alarmTime >> 15) == 1)		// If its enabled
			{
				event = time_to_wake_up;
			}
		}

		interruptCntr = 1;
	}
	else
	{
		Alarm2_IF_Reset();
		interruptCntr++;
	}

	
}