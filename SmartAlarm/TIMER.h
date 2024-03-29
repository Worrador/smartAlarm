﻿/*
 * TIMER.h
 *
 * Created: 2020-02-23 17:11:00
 *  Author: Ákos
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern void config_16bit_timer_to_pwm();
extern void config_16bit_timer_pwm_duty_cycle(uint16_t compareValue);

uint16_t new_currentTime;
typedef enum weektDay{mon = 1, tue, wed, thu, fri, sat, sun, inv, all} weekDay;
weekDay new_currentDay;
weekDay new_alarmDay;
uint16_t new_currentHours;
uint16_t new_currentMinutes;
extern uint16_t secondsCntr;

#endif /* TIMER_H_ */