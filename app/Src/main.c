#include "driver_gpio.h"

static void delay(volatile uint32_t count)
{
    while(count--);
}

int main(void)
{
    gpio_config_t pb5 = {
        .pin        = {GPIOB, GPIO_PIN_5},
        .mode       = GPIO_MODE_OUTPUT,
        .otype      = GPIO_OTYPE_PP,
        .speed      = GPIO_SPEED_HIGH,
        .pull       = GPIO_PULL_UP,
        .init_state = GPIO_STATE_SET
    };
    gpio_init(&pb5);

    while(1)
    {
        gpio_set(pb5.pin);
        delay(500000);
        gpio_clear(pb5.pin);
        delay(500000);
    }
}