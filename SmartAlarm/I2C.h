/*
 * I2C.h
 *
 * Created: 2020-03-01 11:41:17
 *  Author: Ákos
 */ 


#ifndef I2C_H_
#define I2C_H_

// SLA+W transmitted
#define START_OK 0x08
//Master read and repeated START transmitted
#define RPSTART_OK 0x10
//SLA+W transmitted and ACK received
#define SLA_W_ACK 0x18
//SLA+W transmitted and NACK received
#define SLA_W_NACK 0x20
//Data byte transmitted and ACK received
#define DAT_W_ACK 0x28
//Data byte transmitted and NACK received (0x38?)
#define DAT_W_NACK 0x30
//SLA+R transmitted and ACK received
#define SLA_R_ACK 0x40
//SLA+R transmitted and NACK received
#define SLA_R_NACK 0x48
//Data byte received and ACK by master
#define DAT_R_ACK 0x50
//Data byte received and NACK by master
#define DAT_R_NACK 0x58


#define SCL_CLOCK  100000L


extern void config_I2C();
extern uint8_t send_I2C_start(uint8_t slaveAddress);
extern void send_I2C_stop();
extern uint8_t write_I2C_message(uint8_t slaveAddress);
extern uint8_t read_I2C_messageAck();
extern uint8_t read_I2C_messageNak();


#endif /* I2C_H_ */