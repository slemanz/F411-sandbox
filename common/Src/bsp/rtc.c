#include "bsp/rtc.h"

Comm_Interface_t *rtc_comm = NULL;

uint8_t binary_to_bcd(uint8_t value)
{
    uint8_t m, n;
    uint8_t bcd;

    bcd = value;
    if(value >= 10)
    {
        m = value/10;
        n = value % 10;
        bcd = (uint8_t)((m << 4) | n);
    }

    return bcd;
}

uint8_t bcd_to_binary(uint8_t value)
{
    uint8_t m, n;
    uint8_t bin;

    m = (uint8_t)((value >> 4) * 10);
    n = value & (uint8_t)0x0F;

    bin = m + n;
    return bin;
}



void rtc_setup(Comm_Interface_t *comm_protocol)
{
    rtc_comm = comm_protocol;
}

void rtc_set_current_time(RTC_time_t *rtc_time)
{
    
}

void rtc_get_current_time(RTC_time_t *rtc_time)
{
    if(rtc_comm == NULL) return;

    uint8_t write_buffer[2] = {DS3231_I2C_ADDRESS, DS3231_ADDR_SEC};
    uint8_t read_buffer[3];

    rtc_comm->send(write_buffer, 2);

    rtc_comm->send(write_buffer, 1); // read
    rtc_comm->receive(read_buffer, 3);

    rtc_time->seconds = bcd_to_binary(read_buffer[0]);
    rtc_time->minutes = bcd_to_binary(read_buffer[1]);
    rtc_time->hours = bcd_to_binary(read_buffer[2]);
}

void rtc_set_current_date(RTC_date_t *rtc_date)
{
}

void rtc_get_current_date(RTC_date_t *rtc_date)
{
    if(rtc_comm == NULL) return;

    uint8_t write_buffer[2] = {DS3231_I2C_ADDRESS, DS3231_ADDR_DAY};
    uint8_t read_buffer[4];

    rtc_comm->send(write_buffer, 2);

    rtc_comm->send(write_buffer, 1); // read
    rtc_comm->receive(read_buffer, 4);

    rtc_date->day = bcd_to_binary(read_buffer[0]);
    rtc_date->date = bcd_to_binary(read_buffer[1]);
    rtc_date->month = bcd_to_binary(read_buffer[2]);
    rtc_date->year = bcd_to_binary(read_buffer[3]);
}

void rtc_set(RTC_DateTime_t *rtc_dateTime)
{
}

void rtc_get(RTC_DateTime_t *rtc_dateTime)
{
    rtc_get_current_time(&(rtc_dateTime->time));
    rtc_get_current_date(&(rtc_dateTime->date));
}
