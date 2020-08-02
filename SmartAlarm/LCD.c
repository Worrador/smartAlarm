/*
 * LCD.c
 *
 * Created: 2020-03-01 11:36:00
 *  Author: Ákos
 */ 
#include <stdint.h>
#include "LCD.h"
#include "I2C.h"
#include "main.h"
#include <util/delay.h>
#include "UART.h"
#include "TIMER.h"
#include "string.h" 
#include <stdio.h>
#include "RTC.h"

uint8_t colonCounter = 0;
uint16_t LED = 0x08;


void init_LCD()
{
	// Send address+W
	uint8_t temp = 0;
	temp = send_I2C_start(LCD_address);
	_delay_ms(100.0);
		
		
	// Initialization sequence, set 4 bit input mode
	// Send 0x30 3 times and 0x20 one time
	send_LCD_init(0x30);
	send_LCD_init(0x30);
	send_LCD_init(0x30);
	send_LCD_init(0x20);
	
	// 4 bit mode set, specify number of display lines and character font
	send_LCD_config(0x28);
	
	// Display off	
	send_LCD_config(0x08);
	
	// Clear display and set cursor to home	
	send_LCD_config(0x01);
	

	// Set entry mode
	send_LCD_config(0x06);
	
	// Display on, cursor and blinking off
	send_LCD_config(0x0C);
	
	send_I2C_stop();
}

// write one half of LCD data
void send_LCD_init(uint8_t data)
{
	uint8_t temp_data;
	uint8_t temp = 0;
	
	_delay_ms(10.0);
	temp = write_I2C_message(data);
	_delay_ms(10.0);
	temp = write_I2C_message((data | E));
	_delay_ms(10.0);
	temp = write_I2C_message(data);
}

void send_LCD_config(uint8_t data)
{
	uint8_t temp_data;
	uint8_t temp = 0;
	
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED ;				// Cut down the last 4 bits
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED  | E;
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED ;
	temp = write_I2C_message(temp_data);
	
	_delay_ms(5.0);
	temp_data = (data << 4) | LED ;				// Shift the last four bits upward
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data << 4) | LED  | E;
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data << 4) | LED ;
	temp = write_I2C_message(temp_data);
}

void send_LCD_displayData(uint8_t data)
{

	uint8_t temp_data = 0;	
	uint8_t temp = 0;
	
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED | RS;				// Cut down the last 4 bits
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED | E | RS;
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data & 0xF0) | LED | RS;
	temp = write_I2C_message(temp_data);
	
	_delay_ms(5.0);
	temp_data = (data << 4) | LED | RS;				// Shift the last four bits upward
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data << 4) | LED | E | RS;
	temp = write_I2C_message(temp_data);
	_delay_ms(5.0);
	temp_data = (data << 4) | LED | RS;
	temp = write_I2C_message(temp_data);

}

void update_LCD(uint8_t Hour, uint8_t Minutes)
{
	unsigned char data[8];
	unsigned char data2[16];
		
	if((Hour < 10) && (Minutes < 10))
	{
		snprintf(data, sizeof(data), "0%d:0%d", Hour, Minutes);
	}
	else if(Hour < 10)
	{
		snprintf(data, sizeof(data), "0%d:%d", Hour, Minutes);
	}
	else if(Minutes < 10)
	{
		snprintf(data, sizeof(data), "%d:0%d", Hour, Minutes);
	}
	else
	{
		snprintf(data, sizeof(data), "%d:%d", Hour, Minutes);	
	}

	write_LCD_str(data, (LINE1+0x06));
	
	if(refreshAlarm)
	{
		if((c_time.month < 10) && (c_time.day < 10))
		{
			snprintf(data2, sizeof(data2), "   20%d.0%d.0%d.  ", c_time.year, c_time.month, c_time.day);	
		}
		else if(c_time.month < 10)
		{
			snprintf(data2, sizeof(data2), "   20%d.0%d.%d.  ", c_time.year, c_time.month, c_time.day);	
		}
		else if(c_time.day < 10)
		{
			snprintf(data2, sizeof(data2), "   20%d.%d.0%d.  ", c_time.year, c_time.month, c_time.day);	
		}
		else
		{
			snprintf(data2, sizeof(data2), "   20%d.%d.%d.  ", c_time.year, c_time.month, c_time.day);	
		}

		write_LCD_str(data2, LINE2);
	}
	
}


//complete string shorter then 16 char with space
void complete_str_n(char * s, int n)	
{
	int l = strlen(s);
	if(l < n)
	{
		for(int i=0; i<=(n-l); i++ )
		{
			strcat(s," ");
		}
	}	
}

//write string to LCD line
void write_LCD_str(char * s, unsigned char line)
{
	int i = 0;
	// Send address+W
	uint8_t temp = 0;
	temp = send_I2C_start(LCD_address);
	_delay_ms(10.0);
	
	
	send_LCD_config(line);
	while(i < strlen(s))
	{
		send_LCD_displayData(s[i]);
		i++;
	}
	
	send_I2C_stop();
}