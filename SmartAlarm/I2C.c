
/*
 * I2C.c
 *
 * Created: 2020-03-01 11:41:04
 *  Author: Ákos
 */ 
#include <stdint.h> 
#include "I2C.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"

// Max 100 Khz clock frequency -> 16 MHz / (16 + 2*TWBR*prescaler) -> 144 =< 2*TWBR*prescaler -> TWBR >= 9 if prescaler = 8
void config_I2C()
{
	/*TWSR |= (1 << TWPS1);	// Prescaler is 16
	TWBR = 5;				// So TWBR is 5*/
	
	TWSR = 0;                         /* no prescaler */
	TWBR = ((F_CPU_mine/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
}

uint8_t send_I2C_start(uint8_t slaveAddress)
{	
	uint8_t   status;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	status = TWSR & 0xF8;
	if ( (status != START_OK) && (status != RPSTART_OK)) return 1;

	// send device address
	TWDR = slaveAddress;		// Write is zero on last bit
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	status = TWSR & 0xF8;
	if ((status != SLA_W_ACK)) return 1;		//  && (status != SLA_W_NACK) 

	return 0;
}

void send_I2C_stop(void)
{
	/* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// wait until stop condition is executed and bus released
	while(TWCR & (1<<TWSTO));

}

uint8_t write_I2C_message(unsigned char message)
{
    uint8_t   status;
    
    // send data to the previously addressed device
    TWDR = message;
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until transmission completed
    while(!(TWCR & (1<<TWINT)));

    // check value of TWI Status Register. Mask prescaler bits
    status = TWSR & 0xF8;
    if((status == DAT_W_ACK))		//  || (status == DAT_W_NACK)
	{
		return 0;
	}
		
    return 1;

}

uint8_t read_I2C_messageAck(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));

	return TWDR;
}

uint8_t read_I2C_messageNak(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));

	return TWDR;
}
