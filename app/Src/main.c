#include "config.h"

#include "bsp/led.h"
#include "core/simple-timer.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "shared/pool.h"

int main(void)
{
    config_app();

    simple_timer_t timer_blinky;
    simple_timer_setup(&timer_blinky, 500, true);

    uprint("Init the board!\r\n");

    ledPtr_t led1 = led_getByUuid(1);
    ledPtr_t led2 = led_getByUuid(2);

    while(1)
    {
        if(simple_timer_has_elapsed(&timer_blinky))
        {
            led_toggle(led1);
            led_toggle(led2);
        }

        cli_update();
    }
}
