/*
 * UART.h
 *
 * Created: 2020-03-01 11:41:31
 *  Author: Ákos
 */ 


#ifndef UART_H_
#define UART_H_


#define BAUD_RATE_115200_BPS  5 // 115.2k bps

#define BAUD (long)115200	//Baud

#define UBRR_VALUE  (unsigned int)((Fcpu/(16*BAUD)-1) & 0x0fff)

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
extern void config_UART();
extern void send_UART(unsigned char data[]);
extern void clearterminal_UART();

#endif /* UART_H_ */