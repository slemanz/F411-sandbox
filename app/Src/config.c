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

#include "driver_gpio.h"
#include "driver_i2c.h"

const command_t commands_table[] = {
    {"help", cli_help, "List all commands."},
};

void config_core(void)
{
    pool_Init();
    uprint_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2));
    cli_setup(Comm_ProtocolGet(INTERFACE_PROTOCOL_UART2), (command_t*)commands_table, 1);

    // bsp
    ledPtr_t led = led_create("Led 1", IO_Interface_get(INTERFACE_IO_0));
    if(led != NULL)
    {
        led_turn_off(led);
    }

    led = led_create("Led 2", IO_Interface_get(INTERFACE_IO_1));
    if(led != NULL)
    {
        led_invertLogic(led);
        led_turn_off(led);
    }
}

/************************************************************
*                         APP                               *
*************************************************************/

#include "bsp/led.h"

void config_app(void)
{
    config_core();

    // pb6 - I2C1_SCL
    // pb7 - I2C1_SDA
    GPIO_PinConfig_t I2C_pin;
    I2C_pin.pGPIOx = GPIOB;
    I2C_pin.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2C_pin.GPIO_PinPuPdControl = GPIO_PIN_PU;
    I2C_pin.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    I2C_pin.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2C_pin.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_4;

    I2C_pin.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2C_pin);

    I2C_pin.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2C_pin);

    I2C_Config_t I2C_config;
    I2C_config.pI2Cx = I2C1;
    I2C_config.I2C_DeviceAddress = 0x65;
    I2C_config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C_Init(&I2C_config);

    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_ManageAcking(I2C1, ENABLE);
}