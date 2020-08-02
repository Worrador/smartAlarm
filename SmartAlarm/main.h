/*
 * main.h
 *
 * Created: 2020-02-27 12:18:15
 *  Author: Ákos
 */ 


#ifndef MAIN_H_
#define MAIN_H_

void STATE_MACHINE_MAIN();
void update_brigthness();
void config_wake_up();
#define F_CPU 16000000UL
#define F_CPU_mine 16000000UL

typedef enum STATES{IDLE, WAKING_UP, SET_ALARM}STATES;
typedef enum command_word{wake_up, set_alarm, go_idle}command_word;	// Lehet boolba kéne
typedef enum events{nothing, alarm_pressed, capsens_touched, time_to_wake_up, waking_up_ended, message_received} events;

extern command_word command;
extern STATES STATE;
extern volatile events event;
extern uint16_t newCompareValue;

extern volatile unsigned char readData_array[4];
extern volatile uint8_t readData_arrayIndex;

extern volatile _Bool refreshAlarm;
extern volatile int startUp;

#define RTC_address	0xFF
#define LCD_address	0x4E





#endif /* MAIN_H_ */