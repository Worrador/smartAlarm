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

uint8_t interruptCntr = 0;

void config_gpio(){
	// operation led:
	DDRB |= (1 << DDB5);	// pin B5 is output
	
	// pwm output:
	DDRD |= (1 << DDD6);	// pin D6 as pwm output
	
	// alarm setting buttons: hour, quarter hour, minute and then there is the set alarm button:
	DDRB &= ~(1 << DDB0 | 1 << DDB1 | 1 << DDB2 | 1 << DDB4 | 1 << DDB7);			// pins B0-1-2-4 and B7 are inputs
	PORTB |= (1<<PORTB0 | 1<<PORTB1 | 1<<PORTB2 | 1<<PORTB4 | 1<<PORTB7);			// Turn on the pull-up resistors
	
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

ISR (INT0_vect)		// Alarm interrupt
{
	if(interruptCntr == 0)
	{
		event = time_to_wake_up;
		Alarm2_IF_Reset();
		interruptCntr++;
	}
	else
	{
		interruptCntr = 0;
	}
}