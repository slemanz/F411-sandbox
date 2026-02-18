#include "config.h"

/************************************************************
*                       DRIVERS                             *
*************************************************************/
#include "driver_clock.h"


/************************************************************
*                      INTERFACE                            *
*************************************************************/

#include "interface/interface.h"
#include "interface_defines.h"

/************************************************************
*                       COMMON                              *
*************************************************************/
#include "core/uprint.h"
#include "core/cli.h"
#include "core/simple-timer.h"

#include "shared/pool.h"

#include "bsp/led.h"
#include "bsp/button.h"
#include "bsp/rtc.h"


const command_t commands_table[] = {
    {"help", cli_help, "List all commands."},
};

void config_core(void)
{
    pool_Init();
    poolBig_Init();
    uprint_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2));
    cli_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2), (command_t*)commands_table, 1);

    // bsp
    ledPtr_t led = led_create("Led 1", IO_Interface_get(INTERFACE_IO_0));
    if(led != NULL)
    {
        led_invertLogic(led);
        led_turn_off(led);
    }

    led = led_create("Led 2", IO_Interface_get(INTERFACE_IO_1));
    if(led != NULL)
    {
        led_turn_off(led);
    }

    led = led_create("Led Red", IO_Interface_get(INTERFACE_IO_2));
    if(led != NULL)
    {
        led_turn_off(led);
    }

    led = led_create("Led Yellow", IO_Interface_get(INTERFACE_IO_3));
    if(led != NULL)
    {
        led_turn_off(led);
    }

    led = led_create("Led Green", IO_Interface_get(INTERFACE_IO_4));
    if(led != NULL)
    {
        led_turn_off(led);
    }

    buttonPtr_t button = button_create("Button", IO_Interface_get(INTERFACE_IO_5), 10, 500);
    if(button != NULL)
    {
        button_invertLogic(button);
    }


}

/************************************************************
*                         APP                               *
*************************************************************/

#include "bsp/led.h"

void config_app(void)
{
    config_core();
    rtc_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_I2C1));
}