#include "config.h"
#include "interface_defines.h"

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

    ledPtr_t led1 = led_create("Led 1", IO_Interface_get(INTERFACE_IO_0));
    ledPtr_t led2 = led_create("Led 2", IO_Interface_get(INTERFACE_IO_1));
    led_turn_on(led1);

    led_displayInfo(led1);
    led_displayInfo(led2);
    
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
