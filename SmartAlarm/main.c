/*
 * SmartAlarm.c
 *
 * Created: 2020-02-18 14:28:58
 * Author : Ákos
 */ 
#include <stdio.h>
#include "string.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "GPIO.h"
#include "TIMER.h"
#include "BUTTONS.h"
#include <stdint.h> 
#include "RTC.h"
#include "main.h"
#include "LCD.h"
#include <stdbool.h>

#include <avr/eeprom.h>

command_word command = go_idle;
volatile events event = nothing;
STATES STATE = IDLE;
uint16_t newCompareValue = 0;
weekDay new_currentDay = mon;
weekDay new_alarmDay = mon;
volatile unsigned char readData_array[4];
volatile uint8_t readData_arrayIndex = 0;
int messageCounter= 0;
_Bool enableNext = false;
_Bool disableNext = false;
volatile _Bool refreshAlarm;
_Bool UpdateEvery2timesFlag = false;
uint8_t UpdateEvery5times = 0;
uint8_t UpdateEvery10times = 0;
	
volatile int startUp = 0;
int temp = 3;
uint16_t tempTime = 0;


const static struct {
	weekDay      day;
	unsigned char *str;
	} conversion [] = {
	{mon, "mon"},
	{tue, "tue"},
	{wed, "wed"},
	{thu, "thu"},
	{fri, "fri"},
	{sat, "sat"},
	{sun, "sun"}
};

	
weekDay str2enum (unsigned char *str)
{
	int j;
	for (j = 0;  j < 7;  ++j)
	{
		if (!strcmp (str, conversion[j].str))
		{
			return conversion[j].day;
		}
	}
	return inv;
}

unsigned char* enum2str(weekDay day)
{
	int j;
	for (j = 0;  j < 7;  ++j)
	{
		if (day == conversion[j].day)
		{
			return conversion[j].str;
		}
	}
	return "inv";
}

const static struct {
	weekDay     day;
	uint16_t	base_address;
	} conversion2 [] = {
	{mon, 0x00},
	{tue, 0x02},
	{wed, 0x04},
	{thu, 0x06},
	{fri, 0x08},
	{sat, 0x0A},
	{sun, 0x0C}
};


uint16_t enum2base_address(weekDay day)
{
	int j;
	for (j = 0;  j < 7;  ++j)
	{
		if (day == conversion2[j].day)
		{
			return conversion2[j].base_address;
		}
	}

	send_UART(" \n \r Error reading EEPROM, 0x00 returned \n \r");
	return 0x00;
}	

void update_brigthness()
{
	// Every time this function is called the time variable should be incremented by one.
	newCompareValue += 25;
	
	config_16bit_timer_pwm_duty_cycle(newCompareValue);
	if (newCompareValue >= 65000)
	{
		// Temporally:
		newCompareValue = 65535;
		config_16bit_timer_pwm_duty_cycle(newCompareValue);
		event = waking_up_ended;
	}
}

void list_alarms()
{
	int j;
	uint16_t temp2 = 0;
	for (j = 0;  j < 7;  ++j)
	{
		temp2 = eeprom_read_word(conversion2[j].base_address);
		unsigned char* tempday = enum2str(conversion2[j].day);
		unsigned char* temponoff = NULL;
		if((temp2 >> 15) == 1)		// If its enabled
		{
			temponoff = "ON";
		}
		else
		{
			temponoff = "OFF";
		}
		unsigned char tempdata[64];
		snprintf(tempdata, sizeof(tempdata), "\n\r%s : %d:%d \t %s\n\r", tempday,((temp2 >> 8) & 0x007F),(temp2 & 0x00FF), temponoff);
		send_UART(tempdata);
	}
}



void STATE_MACHINE_MAIN()
{
	// State machine:
	switch (STATE)
	{

		case IDLE:
			// IDLE operations:
			// idle_led_toggle();
		
			if(alarm_pressed == event)
			{
				STATE = SET_ALARM;
				event = nothing;
				
				LED |= (1 << 3);

				

				
				// Set the alarm variables to their default on startup:
				if(startUp == 2)
				{
					new_alarmHours = 0;
					new_alarmMinutes = 0;
				}
				else
				{
					startUp = 1;
				}
				
				write_LCD_str(" Set the alarm! ", LINE2);			// Write to LCD
				update_LCD(new_alarmHours, new_alarmMinutes);		// Write out the alarm variables
				enable_alarm_setting_interrupts();					// SET ALARM enter operation
				// PORTB = ~(1 << PORTB5);								// Turn off idle LED
			}
			else if(time_to_wake_up == event)
			{
				STATE = WAKING_UP;
				event = nothing;
				
				PRR &= ~(1 << PRTIM0);				// WAKING UP enter operation, enable pwm output
				//PORTB = ~(1 << PORTB5);				// Turn off idle LED
				newCompareValue = 0;				// Set the compare value to start from zero
				UpdateEvery2timesFlag = false;
				UpdateEvery5times = 0;
				UpdateEvery10times = 0;
				
			}
			else if(capsens_touched == event)		// Set the pwm duty cycle accordingly
			{
				event = nothing;

			}
			else if(message_received == event)
			{
				event = nothing;
				

				if(strcmp(readData_array, "rtc") == 0)
				{
					get_RTC_time();
					Alarm2_Get();
					unsigned char data22[72];
					snprintf(data22,sizeof(data22),"\n\rCurrently set alarm: %s %d:%d\n\rToday is:%s\n\r",enum2str(test.dow),test.hours,test.minutes,enum2str(c_time.dow));
					send_UART(data22);
				}
				if(strcmp(readData_array, "lol") == 0)
				{
					get_RTC_time();
					unsigned char data32[80];
					snprintf(data32,sizeof(data32),"\n\rCurrent time according to the rtc: %s %d:%d:%d\n\r",enum2str(test.dow),c_time.hours,c_time.minutes, c_time.seconds);
					send_UART(data32);
				}
				/*if(strcmp(readData_array, "fos") == 0)
				{
					get_RTC_time();
					c_time.dow = fri;
					c_time.minutes--;
					if(c_time.seconds < 35)
					{
						c_time.minutes--;
						c_time.seconds = c_time.seconds + 25;
					}
					else
					{
						c_time.seconds = c_time.seconds - 35;
					}
					set_RTC(c_time);
				}*/
				
				if(strcmp(readData_array, "ask") == 0)
				{
					clearterminal_UART();		// Clear screen and return cursor
					
					list_alarms();				// List currently set alarms
				}
				else if(strcmp(readData_array, "led") == 0)
				{
					LED ^= (1 << 3);
					update_LCD(new_currentHours, new_currentMinutes);
				}
				else if(strcmp(readData_array, "dis") == 0)
				{
					send_UART("\n\rPlease enter the day you wish the alarm to be disabled on!\n\r");
					disableNext = true;
				}
				else if(strcmp(readData_array, "ena") == 0)
				{
					send_UART("\n\rPlease enter the day you wish the alarm to be enabled on!\n\r");
					enableNext = true;
				}
				else if(strcmp(readData_array, "set") == 0)
				{
					STATE = SET_ALARM;
					send_UART("\n\rPlease set the day of alarm!\n\r");
					messageCounter = 0;
				}
				else if(enableNext || disableNext)
				{
					if(str2enum(readData_array) == inv)		// Set day was invalid all cannot be set, maybe later i will implement it
					{
						send_UART("\n\rInvalid command, please set the day again!\n\r");
					}
					else
					{
						tempTime = eeprom_read_word(enum2base_address(str2enum(readData_array)));						// Read out actual data
						if(enableNext && ((tempTime >> 15) == 0))														// If it should be enabled and it is not enabled right now
						{
							eeprom_write_word(enum2base_address(str2enum(readData_array)), (0x8000 | tempTime));		// Setting enable bit with the same time
							refreshAlarm = true;																		// Refresh the alarms for the current day in main loop
							send_UART("\n\rEnabled.\n\r");
						}
						else if(disableNext && ((tempTime >> 15) == 1))													// If it should be disabled and is not disabled right now
						{
							eeprom_write_word(enum2base_address(str2enum(readData_array)), (0x7FFF & tempTime));		// Resetting enable bit with the same time
							refreshAlarm = true;																		// Refresh the alarms for the current day in main loop
							send_UART("\n\rDisabled.\n\r");
						}
						else
						{
							send_UART("\n\rThe requested configuration was already set.\n\r");
						}
						enableNext = false;
						disableNext = false;
					}
					
				}
				else
				{
					send_UART("\n \rNot valid command received \n \r");
				}
				
			}
		break;	
		

		case SET_ALARM:
				
			
			if(alarm_pressed == event)		// should be set by the other button interrupt
			{
				if((new_currentHours >= 23) || (new_currentHours < 6))		// If the current hour is smaller than 6 or greater than 23 then turn the led off. Else turn it on
				{
					LED &= ~(1 << 3);
				}else
				{
					LED |= (1 << 3);
				}
				disable_alarm_setting_interrupts();			// IDLE enter and SET_ALARM leave operation
				// Set the day of alarm to the next day by default if the current time is smaller than the set alarm time
				if(new_alarmTime < new_currentTime)
				{
					if(new_currentDay != sun)
					{
						new_alarmDay = new_currentDay+1;
					}
					else
					{
						new_alarmDay = mon;
					}
				}
				else
				{
					new_alarmDay = new_currentDay;
					refreshAlarm = true;
				}

				unsigned char data3[64];
				snprintf(data3, sizeof(data3), "\n\rThis day of alarm will be set: %s\n\r", enum2str(new_alarmDay));
				send_UART(data3);

				eeprom_write_word(enum2base_address(new_alarmDay), (0x8000 | new_alarmTime));		// Setting enable bit too
				// Upon using the buttons, the alarm does not need to be set right away because the alarm is set for the next day, every time.
				
				write_LCD_str("   Alarm set.   ", LINE2);
				_delay_ms(2000);
				refreshAlarm = true;
				update_LCD(new_currentHours, new_currentMinutes);
				
				clearterminal_UART();		// Clear screen and return cursor
									
				list_alarms();				// List currently set alarms
				
				
				STATE = IDLE;
				event = nothing;
			}
			else if(capsens_touched == event)
			{
				event = nothing;	

			}
			else if(message_received == event)
			{
				event = nothing;
				if(messageCounter == 0)
				{
					if(str2enum(readData_array) == inv)		// Set day was invalid all cannot be set, maybe lalter i will implement it
					{
						send_UART("\n\rInvalid command, please set the day of alarm again!\n\r");
					}
					else
					{
						messageCounter++;
						new_alarmDay = str2enum(readData_array);
						send_UART("\n\rPlease set the hour of alarm and press enter!\n\r");
					}
				}
				else if(messageCounter == 1)
				{
					temp = sscanf(readData_array, "%d", &new_alarmHours);		// Returns 0 if no number read, 1 if only numbers read and 2 if both number FIRST then characters read
					if((temp == 0) || (new_alarmHours > 23))
					{
						send_UART("\n\rInvalid command, set the hour of alarm again and press enter!\n\r");
					}
					else
					{
						messageCounter++;
						send_UART("\n\rPlease set the minute of alarm and press enter!\n\r");
					}
				}
				else if(messageCounter == 2)
				{
					temp = sscanf(readData_array, "%d", &new_alarmMinutes);
					if((temp == 0) || (new_alarmMinutes > 59))
					{
						send_UART("\n\rInvalid command, set the minute of alarm again and press enter!\n\r");
					}
					else
					{
						messageCounter++;
						new_alarmTime = (new_alarmHours << 8) + new_alarmMinutes;
						
						unsigned char data[100];
						snprintf(data, sizeof(data), "\n\rNew alarm time for the selected day is: %d:%d\n\rTo permanently set this alarm send 'set' again \n\r", new_alarmHours, new_alarmMinutes);
						send_UART(data);

					}
				}	
				else if(strcmp(readData_array, "ask") == 0)
				{
					clearterminal_UART();		// Clear screen and return cursor
								
					// Write currently set alarms:
					list_alarms();
								
				}
				else if(strcmp(readData_array, "set") == 0)
				{
					STATE = IDLE;
					eeprom_write_word(enum2base_address(new_alarmDay), (0x8000 | new_alarmTime));		// Setting enable bit too
					
					disable_alarm_setting_interrupts();			// IDLE enter and SET_ALARM leave operation
					if(new_alarmDay == new_currentDay)
					{
						refreshAlarm = true;						// Refresh the alarms for the current day in main loop
					}
					else
					{
						refreshAlarm = true;
					}
					clearterminal_UART();		// Clear screen and return cursor

				}
				else
				{
					send_UART("Not valid command received \n \r");
				}
			}

			
		break;
		
	
		case WAKING_UP:

			if((waking_up_ended == event) || (capsens_touched == event) || (alarm_pressed == event))		// should be set by the update_brighntess function when the max brightness is set
			{
				STATE = IDLE;
				event = nothing;
				LED |= (1 << 3);			// After waking up i wanna look at the clock
			}
		break;
		
	
		default:
			STATE = IDLE;
			event = nothing;
		break;
	}
}



int main(void)
{
	// Peripheral and interrupt configurations and initializations:
	config_gpio();					// every gpio ever used is configured here
	config_I2C();
	config_UART();					// Config UART or Pc communication
	config_capsense();				// Config capacitive sensor input
	config_set_alarm_interrupt();	// set alarm button interrupt is configured and enabled here
	disable_alarm_setting_interrupts();
	config_16bit_timer_to_pwm();
	
	// Read out actual time from rtc, the next timer interrupt going to write it to the LCD

	get_RTC_time();
	if((new_currentHours >= 23) || (new_currentHours < 9))		// If the current hour is smaller than 6 or greater than 23 then turn the led off. Else turn it on
	{
		LED &= ~(1 << 3);
	}else
	{
		LED |= (1 << 3);
	}


	newCompareValue = 0xFFFF;
	refreshAlarm = true;
	init_LCD();
	update_LCD(new_currentHours, new_currentMinutes);
	IntSqw_Set();					// Set the RTC interrupt output pin to give interrupt
	Alarm2_Set(c_alarm2);
	Alarm2_Enable();
	Alarm2_IF_Reset();


	sei();							// enable every interrupt
	
    while (1)
    {	
		STATE_MACHINE_MAIN();
		if(refreshAlarm == true)
		{
			refreshAlarm = false;
			new_alarmTime = eeprom_read_word(enum2base_address(new_currentDay));		// Read the alarm for the current day
		}
    }
}


