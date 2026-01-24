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

const command_t commands_table[] = {
    {"help", cli_help, "List all commands."},
};

void config_core(void)
{
    pool_Init();
    uprint_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2));
    cli_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2), (command_t*)commands_table, 1);

    // bsp
    if(led_create("Led 1", IO_Interface_get(INTERFACE_IO_0)) == NULL) while(1);
    if(led_create("Led 2", IO_Interface_get(INTERFACE_IO_1)) == NULL) while(1);
    if(led_createWithUuid("Led Teste", IO_Interface_get(INTERFACE_IO_1), 12) == NULL) while(1);

}

/************************************************************
*                         APP                               *
*************************************************************/

#include "bsp/led.h"

void config_app(void)
{
    config_core();
}