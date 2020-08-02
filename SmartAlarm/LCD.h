/*
 * LCD.h
 *
 * Created: 2020-03-01 11:36:25
 *  Author: Ákos
 */ 


#ifndef LCD_H_
#define LCD_H_

//D7 D6 D5 D4 LED E R/W RS
#define RS 0x01
#define E 0x04
//#define LED 0x08

#define CLR_DISP 0x01
#define CR 0x02

#define DDRAM_ADDR 0x80
#define LINE1 DDRAM_ADDR
#define LINE2 (DDRAM_ADDR | 0x40)

extern void update_LCD(uint8_t Hour, uint8_t Minutes);
extern void init_LCD();
extern void test_LCD_clear();
extern void test_LCD_zero();

extern uint8_t colonCounter;
extern uint16_t LED;

#endif /* LCD_H_ */