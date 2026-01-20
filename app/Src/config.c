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
*                         CORE                              *
*************************************************************/
#include "core/uprint.h"
#include "core/cli.h"
#include "core/simple-timer.h"

const command_t commands_table[] = {
    {"help", cli_help, "List all commands."},
};

void config_core(void)
{
    uprint_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2));
    cli_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2), (command_t*)commands_table, 1);
}

/************************************************************
*                         APP                               *
*************************************************************/

#include "bsp/led.h"

void config_app(void)
{
    config_core();
    led_setup(IO_Interface_get(INTERFACE_IO_0));
}