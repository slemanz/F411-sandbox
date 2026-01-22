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

    pool_Init();
    uprint("Mem Pool used: %d\r\n", pool_GetFreeBlockCount());

    int *var_test = pool_Allocate();
    *var_test = 12;
    uprint("teste: %d\r\n", *var_test);
    uprint("Mem Pool used: %d\r\n", pool_GetFreeBlockCount());
    pool_free(var_test);
    
    while(1)
    {
        if(simple_timer_has_elapsed(&timer_blinky))
        {
            led_toggle();
        }

        cli_update();
    }
}
