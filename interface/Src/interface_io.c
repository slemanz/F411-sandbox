#include "interface/interface.h"
#include "interface_instances.h"
#include "interface_defines.h"
#include "driver_gpio.h"

/************************************************************
*                     PIN_DEFAULT                           *
*************************************************************/

#define IO0_INIT_MASK           (1U << 0)
#define IO1_INIT_MASK           (1U << 1)
#define IO2_INIT_MASK           (1U << 2)
#define IO3_INIT_MASK           (1U << 3)
#define IO4_INIT_MASK           (1U << 4)

static uint32_t port_is_init_var = 0;

static uint8_t port_is_init(uint16_t mask)
{
    return (port_is_init_var & mask);
}

/************************************************************
*                         IO0                               *
*************************************************************/

static void io0_init(void)
{
    GPIO_PinConfig_t io1_config;
    io1_config.pGPIOx = GPIOC;
    io1_config.GPIO_PinNumber = GPIO_PIN_NO_13;
    io1_config.GPIO_PinMode = GPIO_MODE_OUT;
    io1_config.GPIO_PinSpeed = GPIO_SPEED_LOW;
    io1_config.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    io1_config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    io1_config.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_0;
    GPIO_Init(&io1_config);

    port_is_init_var |= IO0_INIT_MASK;
}

static void io0_write(uint8_t value)
{
    if(!port_is_init(IO0_INIT_MASK)) io0_init();
    GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_13, value);
}

static uint8_t io0_read(void)
{
    if(!port_is_init(IO0_INIT_MASK)) io0_init();
    return GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13);
}

static void io0_toggle(void)
{
    if(!port_is_init(IO0_INIT_MASK)) io0_init();
    GPIO_ToggleOutputPin(GPIOC, GPIO_PIN_NO_13);
}

/************************************************************
*                         IO1                               *
*************************************************************/

static void io1_init(void)
{
    GPIO_PinConfig_t io1_config;
    io1_config.pGPIOx = GPIOA;
    io1_config.GPIO_PinNumber = GPIO_PIN_NO_5;
    io1_config.GPIO_PinMode = GPIO_MODE_OUT;
    io1_config.GPIO_PinSpeed = GPIO_SPEED_LOW;
    io1_config.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    io1_config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    io1_config.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_0;
    GPIO_Init(&io1_config);

    port_is_init_var |= IO1_INIT_MASK;
}

static void io1_write(uint8_t value)
{
    if(!port_is_init(IO1_INIT_MASK)) io1_init();
    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, value);
}

static uint8_t io1_read(void)
{
    if(!port_is_init(IO1_INIT_MASK)) io1_init();
    return GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_5);
}

static void io1_toggle(void)
{
    if(!port_is_init(IO1_INIT_MASK)) io1_init();
    GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}

/************************************************************
*                         IO2                               *
*************************************************************/

static void io2_init(void)
{
    GPIO_PinConfig_t io_config;
    io_config.pGPIOx = GPIOB;
    io_config.GPIO_PinNumber = GPIO_PIN_NO_3;
    io_config.GPIO_PinMode = GPIO_MODE_OUT;
    io_config.GPIO_PinSpeed = GPIO_SPEED_LOW;
    io_config.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    io_config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    io_config.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_0;
    GPIO_Init(&io_config);

    port_is_init_var |= IO2_INIT_MASK;
}

static void io2_write(uint8_t value)
{
    if(!port_is_init(IO2_INIT_MASK)) io2_init();
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_3, value);
}

static uint8_t io2_read(void)
{
    if(!port_is_init(IO2_INIT_MASK)) io2_init();
    return GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_3);
}

static void io2_toggle(void)
{
    if(!port_is_init(IO2_INIT_MASK)) io2_init();
    GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_3);
}

/************************************************************
*                         IO3                               *
*************************************************************/

static void io3_init(void)
{
    GPIO_PinConfig_t io_config;
    io_config.pGPIOx = GPIOB;
    io_config.GPIO_PinNumber = GPIO_PIN_NO_4;
    io_config.GPIO_PinMode = GPIO_MODE_OUT;
    io_config.GPIO_PinSpeed = GPIO_SPEED_LOW;
    io_config.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    io_config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    io_config.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_0;
    GPIO_Init(&io_config);

    port_is_init_var |= IO3_INIT_MASK;
}

static void io3_write(uint8_t value)
{
    if(!port_is_init(IO3_INIT_MASK)) io3_init();
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_4, value);
}

static uint8_t io3_read(void)
{
    if(!port_is_init(IO3_INIT_MASK)) io3_init();
    return GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_4);
}

static void io3_toggle(void)
{
    if(!port_is_init(IO3_INIT_MASK)) io3_init();
    GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_4);
}

/************************************************************
*                         IO4                               *
*************************************************************/

static void io4_init(void)
{
    GPIO_PinConfig_t io_config;
    io_config.pGPIOx = GPIOB;
    io_config.GPIO_PinNumber = GPIO_PIN_NO_5;
    io_config.GPIO_PinMode = GPIO_MODE_OUT;
    io_config.GPIO_PinSpeed = GPIO_SPEED_LOW;
    io_config.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    io_config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    io_config.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_0;
    GPIO_Init(&io_config);

    port_is_init_var |= IO4_INIT_MASK;
}

static void io4_write(uint8_t value)
{
    if(!port_is_init(IO4_INIT_MASK)) io4_init();
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_5, value);
}

static uint8_t io4_read(void)
{
    if(!port_is_init(IO4_INIT_MASK)) io4_init();
    return GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_5);
}

static void io4_toggle(void)
{
    if(!port_is_init(IO4_INIT_MASK)) io4_init();
    GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_5);
}

/************************************************************
*                    INSTANCES
*************************************************************/

const IO_Interface_t io0_pin = {
    .init = io0_init,
    .read = io0_read,
    .write = io0_write,
    .toggle = io0_toggle,
    .deinit = NULL
};

const IO_Interface_t io1_pin = {
    .init = io1_init,
    .read = io1_read,
    .write = io1_write,
    .toggle = io1_toggle,
    .deinit = NULL
};

const IO_Interface_t io2_pin = {
    .init = io2_init,
    .read = io2_read,
    .write = io2_write,
    .toggle = io2_toggle,
    .deinit = NULL
};

const IO_Interface_t io3_pin = {
    .init = io3_init,
    .read = io3_read,
    .write = io3_write,
    .toggle = io3_toggle,
    .deinit = NULL
};

const IO_Interface_t io4_pin = {
    .init = io4_init,
    .read = io4_read,
    .write = io4_write,
    .toggle = io4_toggle,
    .deinit = NULL
};

/************************************************************
*                       GET
*************************************************************/

IO_Interface_t *IO_Interface_get(uint8_t pin)
{
    switch (pin)
    {
        case INTERFACE_IO_0: return (IO_Interface_t*)&io0_pin;
        case INTERFACE_IO_1: return (IO_Interface_t*)&io1_pin;
        case INTERFACE_IO_2: return (IO_Interface_t*)&io2_pin;
        case INTERFACE_IO_3: return (IO_Interface_t*)&io3_pin;
        case INTERFACE_IO_4: return (IO_Interface_t*)&io4_pin;
        
        default:
            return NULL;
    }
}