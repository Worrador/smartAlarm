/*
 * RTC.h
 *
 * Created: 2020-03-01 11:36:38
 *  Author: Ákos
 */ 


#ifndef RTC_H_
#define RTC_H_

#define DS3231_ADDRESS       0xD0
#define DS3231_REG_SECONDS   0x00
#define DS3231_REG_AL1_SEC   0x07
#define DS3231_REG_AL2_MIN   0x0B
#define DS3231_REG_CONTROL   0x0E
#define DS3231_REG_STATUS    0x0F
#define DS3231_REG_TEMP_MSB  0x11

typedef enum
{
	MONDAY = 1,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY
} RTC_DOW;

typedef enum
{
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
} RTC_Month;

typedef struct rtc_tm
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	RTC_DOW dow;
	uint8_t day;
	RTC_Month month;
	uint8_t year;
} RTC_Time;

typedef enum
{
	ONCE_PER_SECOND = 0x0F,
	SECONDS_MATCH = 0x0E,
	MINUTES_SECONDS_MATCH = 0x0C,
	HOURS_MINUTES_SECONDS_MATCH = 0x08,
	DATE_HOURS_MINUTES_SECONDS_MATCH = 0x0,
	DAY_HOURS_MINUTES_SECONDS_MATCH = 0x10
} al1;

typedef enum
{
	ONCE_PER_MINUTE = 0x0E,
	MINUTES_MATCH = 0x0C,
	HOURS_MINUTES_MATCH = 0x08,
	DATE_HOURS_MINUTES_MATCH = 0x0,
	DAY_HOURS_MINUTES_MATCH = 0x10
} al2;

typedef enum
{
	OUT_OFF = 0x00,
	OUT_INT = 0x04,
	OUT_1Hz = 0x40,
	OUT_1024Hz = 0x48,
	OUT_4096Hz = 0x50,
	OUT_8192Hz = 0x58
} INT_SQW;

RTC_Time c_time, c_alarm1, c_alarm2, test;

extern void get_RTC_time();
extern void set_RTC(RTC_Time time_t);
extern void Alarm2_Enable();
extern void Alarm2_Disable();
extern void IntSqw_Set();
extern void Alarm2_Set(RTC_Time time_t);
extern void Alarm2_Get();
extern void Alarm2_IF_Reset();

RTC_Time RTC_CONFIG_TIME;




#endif /* RTC_H_ */