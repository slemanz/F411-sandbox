#include "config.h"

#include "core/simple-timer.h"
#include "core/ticker.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "core/fault.h"

#include "bsp/led.h"
#include "bsp/rtc.h"
#include "bsp/button.h"


static void task_blinky(void)
{
    led_toggle(led_getByUuid(1));
    led_toggle(led_getByUuid(3));
}

static void task_button(void)
{
    button_update(button_getByUuid(1));
}

static const ticker_task_t app_tasks[] = {
    TICKER_TASK(task_blinky,  500),
    TICKER_TASK(task_button,  10),
};

int main(void)
{
    config_app();

    ticker_init(app_tasks, TICKER_TASK_COUNT(app_tasks));

    uprint("Init the board!\r\n");

    while(1)
    {
        ticker_update();
        cli_update();
        fault_update();
    }
}
