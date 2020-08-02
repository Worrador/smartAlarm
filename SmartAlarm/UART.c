/*
 * UART.c
 *
 * Created: 2020-03-01 11:41:40
 *  Author: Ákos
 */ 
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include "main.h"

config_UART()
{
	unsigned int ubrr = MYUBRR;
	
	UBRR0 = 0;
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);				// Set frame format: 8data, 1stop bit no parity
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<< RXCIE0);	// Enable transmitter
	UBRR0H = (ubrr>>8);								// Shift the 16bit value ubrr 8 times to the right and transfer the upper 8 bits to UBBR0H register.
	UBRR0L = (ubrr);								// Copy the 16 bit value ubrr to the 8 bit UBBR0L register,Upper 8 bits are truncated while lower 8 bits are copied
}

send_UART(unsigned char data[])
{
	int i = 0;
	while(data[i] != 0)								// print the String  "Hello from ATmega328p"
	{
		while (!( UCSR0A & (1<<UDRE0)));			// Wait for empty transmit buffer

		UDR0 = data[i];								// Put data into buffer, sends the data
		i++;										// increment counter
	}

	_delay_ms(100);
}

clearterminal_UART()
{
	send_UART("\033[2J\033[H");
}


ISR(USART_RX_vect)
{
	unsigned char data[2];
		
	// Read in 3 characters sequentially:
	readData_array[readData_arrayIndex] = UDR0;
	
	// Respond with the input
	snprintf(data, sizeof(data), "%c", readData_array[readData_arrayIndex]);
	send_UART(data);
	
	
	readData_arrayIndex++;
	
	if(readData_arrayIndex >= 3)
	{
		event = message_received;				// Send signal to main code that 3 characters have been received
		readData_arrayIndex = 0;
	}
	
}
