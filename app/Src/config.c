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
#include "core/fault.h"

void config_fault(void);

void config_app(void)
{
    config_core();
    rtc_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_I2C1));
    config_fault();
}

/************************************************************
*                       FAULTS                              *
*************************************************************/

static bool detect_overcurrent_output1(void)
{
    if(button_isPressed(button_getByUuid(1))) // must be update in main()
    {
        return true;
    }
    return false; 
}

static void action_overcurrent_output1(void)
{
    led_turn_on(led_getByUuid(3));
    uprint("[APP] Output 1 disabled due to overcurrent.\r\n");
}

static void recover_overcurrent_output1(void)
{
    led_turn_off(led_getByUuid(3));
    uprint("[APP] Output 1 re-enabled after cooldown.\r\n");
}

#define RECOVERY_1_MIN_MS  (1UL * 60UL * 1000UL)

static fault_handle_t h_overcurrent_out1 = NULL;

void config_fault(void)
{
    fault_init();
    /* Overcurrent on output 1: auto-recovery after 10 minutes */
    const fault_config_t overcurrent_cfg = {
        .name         = "Overcurrent Out1",
        .detect       = detect_overcurrent_output1,
        .on_fault     = action_overcurrent_output1,
        .on_recover   = recover_overcurrent_output1, // can be null if no recover action is needed
        .recovery_ms  = RECOVERY_1_MIN_MS,
        //.recovery_ms  = 5000,
    };
    h_overcurrent_out1 = fault_register(&overcurrent_cfg);
    (void)h_overcurrent_out1;
}