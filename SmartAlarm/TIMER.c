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


void config_16bit_timer_to_pwm()
{	
	// This function sets the timer1 to pwm mode:
	//OCR1A = 0xFFFF;
	ICR1 = 0xFFFF;
	// set TOP to 16bit
	
	TCCR1A = (1<<COM1B1) | (0<<COM1B0) | (1<<WGM11);
	TCCR1B = (1 << CS10)|(1 << WGM12)|(1 << WGM13);
	
	// Disable the timer on power up:
	OCR1B = 0;
	PRR |= (1 << PRTIM1);
	
    //TIMSK1 = 0;					// Set interrupt on compare match
	
}

void config_16bit_timer_pwm_duty_cycle(uint16_t compareValue)
{
	OCR1B = compareValue;
}

