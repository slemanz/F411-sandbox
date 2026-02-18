#include "config.h"

#include "core/simple-timer.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "bsp/led.h"
#include "bsp/rtc.h"
#include "bsp/button.h"

//RTC_DateTime_t rtc;

int main(void)
{
    config_app();

    simple_timer_t timer_blinky;
    simple_timer_t timer_rtc;
    simple_timer_setup(&timer_blinky, 500, true);
    simple_timer_setup(&timer_rtc, 60000, true);

    uprint("Init the board!\r\n");

    ledPtr_t led1 = led_getByUuid(1);
    ledPtr_t led2 = led_getByUuid(2);
    ledPtr_t led3 = led_getByUuid(3);

    //rtc_get(&rtc);

    buttonPtr_t button = button_getByUuid(1);

    while(1)
    {
        if(simple_timer_has_elapsed(&timer_blinky))
        {
            led_toggle(led1);
            led_toggle(led2);
        }

        button_update(button);
        if(button_isPressed(button))
        {
            led_turn_on(led3);
        }else
        {
            led_turn_off(led3);
        }

        if(simple_timer_has_elapsed(&timer_rtc))
        {
            //rtc_get(&rtc);
            //uprint("%d/%d/%d - %d:%d:%d\r\n", rtc.date.date, rtc.date.month, rtc.date.year, rtc.time.hours, rtc.time.minutes, rtc.time.seconds);
        }

        cli_update();
    }
}
