#include "config.h"

#include "core/simple-timer.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "bsp/led.h"
#include "bsp/rtc.h"

#include "driver_i2c.h"

RTC_DateTime_t rtc;

int main(void)
{
    config_app();

    simple_timer_t timer_blinky;
    simple_timer_setup(&timer_blinky, 1000, true);

    uprint("Init the board!\r\n");

    ledPtr_t led1 = led_getByUuid(1);
    ledPtr_t led2 = led_getByUuid(2);

    while(1)
    {
        if(simple_timer_has_elapsed(&timer_blinky))
        {
            led_toggle(led1);
            led_toggle(led2);
            rtc_get(&rtc);

            uprint("%d/%d/%d - %d:%d:%d\r\n", rtc.date.date, rtc.date.month, rtc.date.year,
                                              rtc.time.hours, rtc.time.minutes, rtc.time.seconds);
        }

        cli_update();
    }
}
