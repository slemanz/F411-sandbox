#include "driver_gpio.h"

/*
 * Private Helpers
 */

static uint32_t gpio_clock_bit(GPIO_TypeDef *port)
{
    if      (port == GPIOA) return RCC_AHB1ENR_GPIOAEN;
    else if (port == GPIOB) return RCC_AHB1ENR_GPIOBEN;
    else if (port == GPIOC) return RCC_AHB1ENR_GPIOCEN;
    else if (port == GPIOD) return RCC_AHB1ENR_GPIODEN;
    else if (port == GPIOE) return RCC_AHB1ENR_GPIOEEN;
    else if (port == GPIOH) return RCC_AHB1ENR_GPIOHEN;

    return 0;
}

/*
 * Clock control
 */

void gpio_clock_enable(GPIO_TypeDef *port)
{
    RCC->AHB1ENR |= gpio_clock_bit(port);
}

void gpio_clock_disable(GPIO_TypeDef *port)
{
    RCC->AHB1ENR &= ~gpio_clock_bit(port);
}

/*
 * Pin helper (runtime)
 */

gpio_pin_t gpio_pin(GPIO_TypeDef *port, gpio_pin_number_e number)
{
    gpio_pin_t pin;

    pin.port   = port;
    pin.number = number;

    return pin;
}

/*
 * Init and De-init
 */

/*
 * Configuration
 */

/*
 * Data read and write
 */
