/*
 * RTC.c
 *
 * Created: 2020-03-01 11:36:10
 *  Author: Ákos
 */ 
#include <stdint.h>
#include "RTC.h"
#include <util/delay.h>
#include "TIMER.h"
#include "UART.h"


// converts BCD to decimal
uint8_t bcd_to_decimal(uint8_t number)
{
	return ( (number >> 4) * 10 + (number & 0x0F) );
}

// converts decimal to BCD
uint8_t decimal_to_bcd(uint8_t number)
{
	return ( ((number / 10) << 4) + (number % 10) );
}

// sets time and date
void set_RTC(RTC_Time time_t)
{
	// convert decimal to BCD
	time_t.day     = decimal_to_bcd(time_t.day);
	time_t.month   = decimal_to_bcd(time_t.month);
	time_t.year    = decimal_to_bcd(time_t.year);
	time_t.hours   = decimal_to_bcd(time_t.hours);
	time_t.minutes = decimal_to_bcd(time_t.minutes);
	time_t.seconds = decimal_to_bcd(time_t.seconds);
	// end conversion

	// write data to the RTC chip
	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_SECONDS);
	_delay_ms(5);
	write_I2C_message(time_t.seconds);
	_delay_ms(5);
	write_I2C_message(time_t.minutes);
	_delay_ms(5);
	write_I2C_message(time_t.hours);
	_delay_ms(5);
	write_I2C_message(time_t.dow);
	_delay_ms(5);
	write_I2C_message(time_t.day);
	_delay_ms(5);
	write_I2C_message(time_t.month);
	_delay_ms(5);
	write_I2C_message(time_t.year);
	send_I2C_stop();
}

// reads time and date
void get_RTC_time()
{
	// First write to the pointer
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_SECONDS);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	c_time.seconds = read_I2C_messageAck();
	c_time.minutes = read_I2C_messageAck();
	c_time.hours   = read_I2C_messageAck();
	c_time.dow   = read_I2C_messageAck();
	c_time.day   = read_I2C_messageAck();
	c_time.month = read_I2C_messageAck();
	c_time.year  = read_I2C_messageNak();
	send_I2C_stop();

	// convert BCD to decimal
	c_time.seconds = bcd_to_decimal(c_time.seconds);
	c_time.minutes = bcd_to_decimal(c_time.minutes);
	c_time.hours   = bcd_to_decimal(c_time.hours);
	c_time.day     = bcd_to_decimal(c_time.day);
	c_time.month   = bcd_to_decimal(c_time.month);
	c_time.year    = bcd_to_decimal(c_time.year);
	
	new_currentDay = c_time.dow;
	new_currentMinutes = c_time.minutes;
	new_currentHours = c_time.hours;
	secondsCntr = c_time.seconds;
}


// sets alarm2 details
void Alarm2_Set(RTC_Time time_t)
{	
	// convert decimal to BCD
	uint8_t hours   = decimal_to_bcd(time_t.hours);
	uint8_t minutes = decimal_to_bcd(time_t.minutes);
	// end conversion
	
	// write data to the RTC chip
	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_AL2_MIN);
	_delay_ms(5);
	write_I2C_message(minutes);
	_delay_ms(5);
	write_I2C_message(hours);
	_delay_ms(5);
	write_I2C_message(time_t.dow | 0x40 | (1 << 7));

	send_I2C_stop();
}



void IntSqw_Set()
{
	INT_SQW _config = OUT_INT;
	
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_CONTROL);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	uint8_t ctrl_reg = read_I2C_messageNak();
	send_I2C_stop();

	ctrl_reg &= 0xA3;
	ctrl_reg |= _config;
	
	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_CONTROL);
	_delay_ms(5);
	write_I2C_message(ctrl_reg);
	send_I2C_stop();
}

// enables alarm2
void Alarm2_Enable()
{
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_CONTROL);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	uint8_t ctrl_reg = read_I2C_messageNak();
	send_I2C_stop();
	
	ctrl_reg |= 0x02;
	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_CONTROL);
	_delay_ms(5);
	write_I2C_message(ctrl_reg);
	send_I2C_stop();
}

void Alarm2_Disable()
{
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_CONTROL);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	uint8_t ctrl_reg = read_I2C_messageNak();
	send_I2C_stop();
	
	ctrl_reg &= 0xFD;
	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_CONTROL);
	_delay_ms(5);
	write_I2C_message(ctrl_reg);
	send_I2C_stop();
}

Alarm2_Get()
{
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_AL2_MIN);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	test.minutes = read_I2C_messageAck() & 0x7F;
	test.hours   = read_I2C_messageAck() & 0x3F;
	test.dow = read_I2C_messageNak() & 0x30F;
	send_I2C_stop();


	// convert BCD to decimal
	test.minutes = bcd_to_decimal(test.minutes);
	test.hours   = bcd_to_decimal(test.hours);
	// end conversion
}

// resets alarm2 flag bit
void Alarm2_IF_Reset()
{
	send_I2C_start(DS3231_ADDRESS);
	write_I2C_message(DS3231_REG_STATUS);
	send_I2C_start(DS3231_ADDRESS | 0x01);
	uint8_t stat_reg = read_I2C_messageNak();
	send_I2C_stop();
	
	stat_reg &= 0xFD;
  	send_I2C_start(DS3231_ADDRESS);
	_delay_ms(5);
	write_I2C_message(DS3231_REG_STATUS);
	_delay_ms(5);
	write_I2C_message(stat_reg);
	send_I2C_stop();
}