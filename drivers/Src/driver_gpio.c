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
 * Configuration
 */

void gpio_set_mode(gpio_pin_t pin, gpio_mode_e mode)
{
    pin.port->MODER &= ~(0x3U << (pin.number * 2));
    pin.port->MODER |=  ((uint32_t)mode << (pin.number * 2));
}

void gpio_set_pull(gpio_pin_t pin, gpio_pull_e pull)
{
    pin.port->PUPDR &= ~(0x3U << (pin.number * 2));
    pin.port->PUPDR |=  ((uint32_t)pull << (pin.number * 2));
}

void gpio_set_otype(gpio_pin_t pin, gpio_otype_e otype)
{
    pin.port->OTYPER &= ~(0x1U << pin.number);
    pin.port->OTYPER |=  ((uint32_t)otype << pin.number);
}

void gpio_set_speed(gpio_pin_t pin, gpio_speed_e speed)
{
    pin.port->OSPEEDR &= ~(0x3U << (pin.number * 2));
    pin.port->OSPEEDR |=  ((uint32_t)speed << (pin.number * 2));
}

void gpio_set_af(gpio_pin_t pin, gpio_af_e af)
{
}

/*
 * Init and De-init
 */

/*
 * Data read and write
 */
