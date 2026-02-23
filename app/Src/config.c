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
#include "core/ticker.h"
#include "core/fault.h"
#include "driver_systick.h"

#include "shared/pool.h"

#include "bsp/led.h"
#include "bsp/button.h"
#include "bsp/rtc.h"
#include "bsp/output.h"


static void cmd_status(void);
static void cmd_leds(void);
static void cmd_outputs(void);
static void cmd_adc(void);
static void cmd_faults(void);
static void cmd_uptime(void);
static void cmd_rtc(void);

static void cmd_pool(void);

const command_t commands_table[] = {
    {"help",   cli_help,           "List all commands"},
    {"status", cmd_status,         "System status overview"},
    {"leds",   cmd_leds,           "List all LEDs"},
    {"outputs",cmd_outputs,        "List all PWM outputs"},
    {"adc",    cmd_adc,            "Read ADC0 (PA1, 12-bit)"},
    {"faults", cmd_faults,         "Show fault status"},
    {"uptime", cmd_uptime,         "Show system uptime"},
    {"rtc",    cmd_rtc,            "Show rtc time"},
    {"pool",   cmd_pool,           "Show memory pool usage"},
};

#define COMMANDS_COUNT (sizeof(commands_table) / sizeof(commands_table[0]))

void config_core(void)
{
    pool_Init();
    poolBig_Init();
    uprint_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2));
    cli_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2), (command_t*)commands_table, COMMANDS_COUNT);

    // bsp
    ledPtr_t led = led_create("Led 1", IO_Interface_get(INTERFACE_IO_0));
    if(led != NULL)
    {
        led_invertLogic(led);
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

    outputPtr_t out = output_create("Output 1", PWM_Interface_get(INTERFACE_PWM_0));
    if (out != NULL)
    {
        output_set(out, 30U);
    }
}

/************************************************************
*                         APP                               *
*************************************************************/

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

/************************************************************
*                     CLI COMMANDS                          *
*************************************************************/

static void cmd_status(void)
{
    uint64_t ms = ticks_get();
    uint32_t sec = (uint32_t)(ms / 1000);
    uint32_t min = sec / 60;
    uint32_t hrs = min / 60;

    uprint("=== System Status ===\r\n");
    uprint("Uptime: %uh %um %us\r\n", hrs, min % 60, sec % 60);
    uprint("Pool free: %u/%u  Big: %u/%u\r\n",
           pool_GetFreeBlockCount(), POOL_BLOCK_COUNT,
           poolBig_GetFreeBlockCount(), POOL_BIG_BLOCK_COUNT);
    uprint("Faults active: %s\r\n", fault_any_active() ? "YES" : "no");
    uprint("=====================\r\n");
}

static void cmd_leds(void)
{
    led_displayAll();
}

static void cmd_outputs(void)
{
    output_displayAll();
}

static void cmd_adc(void)
{
    ADC_Interface_t *adc = ADC_Interface_get(INTERFACE_ADC_0);
    if (adc == NULL)
    {
        uprint("[ADC] Interface not available.\r\n");
        return;
    }

    uint16_t raw     = adc->read();
    uint32_t mv      = (raw * 3300UL) / 4095UL;

    uprint("ADC0 (PA1): raw=%u  voltage=%u mV\r\n", raw, mv);
}

static void cmd_faults(void)
{
    fault_print_status();
}

static void cmd_uptime(void)
{
    uint64_t ms = ticks_get();
    uint32_t sec = (uint32_t)(ms / 1000);
    uint32_t min = sec / 60;
    uint32_t hrs = min / 60;
    uint32_t days = hrs / 24;

    uprint("%ud %uh %um %us\r\n", days, hrs % 24, min % 60, sec % 60);
}

static void cmd_pool(void)
{
    uprint("Standard pool: %u/%u blocks free (%u bytes each)\r\n",
           pool_GetFreeBlockCount(), POOL_BLOCK_COUNT, POOL_BLOCK_SIZE);
    uprint("Big pool:      %u/%u blocks free (%u bytes each)\r\n",
           poolBig_GetFreeBlockCount(), POOL_BIG_BLOCK_COUNT, POOL_BIG_BLOCK_SIZE);
}

static void cmd_rtc(void)
{
    RTC_DateTime_t rtc;
    rtc_get(&rtc);
    uprint("%d/%d/%d - %d:%d:%d\r\n", rtc.date.date, rtc.date.month, rtc.date.year,
                                rtc.time.hours, rtc.time.minutes, rtc.time.seconds);

}