#ifndef INC_RTC_H_
#define INC_RTC_H_

#include <stdint.h>

/* Register address */
#define DS3231_ADDR_SEC         0x00
#define DS3231_ADDR_MIN         0x01
#define DS3231_ADDR_HRS         0x02
#define DS3231_ADDR_DAY         0x03
#define DS3231_ADDR_DATE        0x04
#define DS3231_ADDR_MONTH       0x05
#define DS3231_ADDR_YEAR        0x06

// sunday can be defined as any day, only keep logic sequential!
#define SUNDAY		1;
#define MONDAY		2;
#define TUESDAY		3;
#define WEDNESDAY	4;
#define THURSDAY	5;
#define FRIDAY		6;
#define SATURDAY	7;

typedef struct
{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;
}RTC_date_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;

uint8_t binary_to_bcd(uint8_t value);
uint8_t bcd_to_binary(uint8_t value);


#endif /* INC_RTC_H_ */