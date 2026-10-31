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
    uint8_t reg = pin.number / 8;          /* AFR[0]: pins 0..7, AFR[1]: pins 8..15 */
    uint8_t pos = (pin.number % 8) * 4;    /* 4 bits per pin */

    pin.port->AFR[reg] &= ~(0xFU << pos);
    pin.port->AFR[reg] |=  ((uint32_t)af << pos);
}

/*
 * Init and De-init
 */

void gpio_init(const gpio_config_t *config)
{
    gpio_clock_enable(config->pin.port);

    if(config->mode == GPIO_MODE_OUTPUT)
    {
        gpio_write(config->pin, config->init_state);
    }

    gpio_set_otype(config->pin, config->otype);
    gpio_set_speed(config->pin, config->speed);
    gpio_set_pull(config->pin, config->pull);
    gpio_set_af(config->pin, config->af);
    gpio_set_mode(config->pin, config->mode);
}

void gpio_init_table(const gpio_config_t *table, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        gpio_init(&table[i]);
    }
}

void gpio_deinit(gpio_pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_INPUT);
    gpio_set_otype(pin, GPIO_OTYPE_PP);
    gpio_set_speed(pin, GPIO_SPEED_LOW);
    gpio_set_pull(pin, GPIO_PULL_NONE);
    gpio_set_af(pin, GPIO_AF0);
    gpio_clear(pin);
}

void gpio_port_reset(GPIO_TypeDef *port)
{
    uint32_t reset_bit = 0;

    if      (port == GPIOA) reset_bit = RCC_AHB1RSTR_GPIOARST;
    else if (port == GPIOB) reset_bit = RCC_AHB1RSTR_GPIOBRST;
    else if (port == GPIOC) reset_bit = RCC_AHB1RSTR_GPIOCRST;
    else if (port == GPIOD) reset_bit = RCC_AHB1RSTR_GPIODRST;
    else if (port == GPIOE) reset_bit = RCC_AHB1RSTR_GPIOERST;
    else if (port == GPIOH) reset_bit = RCC_AHB1RSTR_GPIOHRST;

    RCC->AHB1RSTR |=  reset_bit;
    RCC->AHB1RSTR &= ~reset_bit;
}

/*
 * Data read and write
 */

void gpio_set(gpio_pin_t pin)
{
    pin.port->BSRR = (1U << pin.number);
}

void gpio_clear(gpio_pin_t pin)
{
    pin.port->BSRR = (1U << (pin.number + 16));
}

void gpio_write(gpio_pin_t pin, gpio_state_e state)
{
    if(state == GPIO_STATE_SET)
    {
        gpio_set(pin);
    }else
    {
        gpio_clear(pin);
    }
}

gpio_state_e gpio_read(gpio_pin_t pin)
{
    if(pin.port->IDR & (1U << pin.number))
    {
        return GPIO_STATE_SET;
    }
    return GPIO_STATE_RESET;
}

void gpio_toggle(gpio_pin_t pin)
{
    if(pin.port->ODR & (1U << pin.number))
    {
        gpio_clear(pin);
    }else
    {
        gpio_set(pin);
    }
}