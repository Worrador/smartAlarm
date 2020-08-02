/*
 * PUSHBUTTONS.h
 *
 * Created: 2020-02-23 18:02:44
 *  Author: Ákos
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_


extern void config_set_alarm_interrupt();
extern void enable_alarm_setting_interrupts();
extern void disable_alarm_setting_interrupts();
extern void config_button_interrupts();

uint16_t new_alarmTime;
uint16_t new_alarmHours;
uint16_t new_alarmMinutes;

uint8_t presscounter;

extern volatile uint8_t portbhistory;     // default is high because the pull-up

#endif /* PUSHBUTTONS_H_ */