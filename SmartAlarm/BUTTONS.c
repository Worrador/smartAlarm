/*
 * PUSHBUTTONS.c
 *
 * Created: 2020-02-23 17:40:54
 *  Author: Ákos
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "BUTTONS.h"
#include "LCD.h"
#include <stdbool.h> 
#include <util/delay.h>

volatile uint8_t portbhistory = 0xFF;
uint8_t presscounter = 0;
uint16_t new_alarmMinutes = 0;
volatile bool notFirst = false;


void config_set_alarm_interrupt()				// Maybe only falling edge is needed
{
	PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan = the configured interrupts in the PCMSK0 register are enabled
	PCMSK0 |= (1 << PCINT7);   // set the PB7 (PUSH BUTTON) to trigger an interrupt on state change
}

void enable_alarm_setting_interrupts()			// Maybe only falling edge is needed
{
	PCMSK0 |= (1 << PCINT0 | 1 << PCINT2 | 1 << PCINT4);   // set PCINT0-1-2 to trigger an interrupt on state change, maybe the first is enough
}

void disable_alarm_setting_interrupts()			// Maybe only falling edge is needed
{  
	PCMSK0 &= ~(1 << PCINT0 | 1 << PCINT2 | 1 << PCINT4);
}

void config_capsense()
{
	PCMSK0 |= (1 << PCINT1);   // set the PB1 (capsens) to trigger an interrupt on state change
}


ISR (PCINT0_vect)		// Ha nem megy akkor kell majd egy timer arra hogy nézze h fél secig ne fogadjunk el interruptot vagy ilyesmi
{
	uint8_t changedbits;


	changedbits = PINB ^ portbhistory;
	portbhistory = PINB;
	

	if(changedbits & (1 << PINB7))
	{
		if(presscounter == 0)
		{
			event = alarm_pressed;
			presscounter++;
		}else
		{
			presscounter = 0;
		}
		
	}
	else if(((changedbits & (1 << PINB0)) || (changedbits & (1 << PINB2)) || (changedbits & (1 << PINB3))) && (STATE == SET_ALARM))
	{
		if(changedbits & (1 << PINB0))		// Hour
		{
			
			/* PCINT0 changed */
			if(presscounter == 0)
			{
				new_alarmHours = (new_alarmHours < 23) ? (new_alarmHours + 1) : 0;
				presscounter++;
			}else
			{
				presscounter = 0;
			}
		}
			
		else if(changedbits & (1 << PINB2))		// Quarter hours
		{
						
			/* PCINT2 changed */
			if(presscounter == 0)
			{
				new_alarmMinutes = (new_alarmMinutes < 45) ? (new_alarmMinutes + 15) : (new_alarmMinutes - 45);
				presscounter++;
			}else
			{
				presscounter = 0;
			}
		}

		else if(changedbits & (1 << PINB4))		// Minutes
		{
			/* PCINT3 changed */
			if(presscounter == 0)
			{
				new_alarmMinutes = (new_alarmMinutes < 59) ? (new_alarmMinutes + 1) : 0;
				presscounter++;
			}else
			{
				presscounter = 0;
			}

		}
			
		new_alarmTime = (new_alarmHours << 8) + new_alarmMinutes;
		update_LCD(new_alarmHours, new_alarmMinutes);
	}
	else if(changedbits & (1 << PINB1))
	{
		if(presscounter == 0)
		{
			event = capsens_touched;
			presscounter++;
			if(newCompareValue == 255)
			{
				
				newCompareValue = 0;
				config_pwm_timer(newCompareValue);
				_delay_ms(10);
				PRR |= (1 << PRTIM0);
			}
			else
			{
				if(newCompareValue < 10)
				{
					newCompareValue = 10;
				}
				else if(newCompareValue < 55)
				{
					newCompareValue = 55;
				}
				else if(newCompareValue < 110)
				{
					newCompareValue = 110;
				}
				else if(newCompareValue < 175)
				{
					newCompareValue = 175;
				}
				else
				{
					newCompareValue = 255;
				}
				PRR &= ~(1 << PRTIM0);
				config_pwm_timer(newCompareValue);
			}
		}else
		{
			presscounter = 0;
		}
		
	}
	notFirst = true;
}