#include "driver_gpio.h"
#include "driver_systick.h"

#define CORE_CLOCK_HZ   16000000U
#define BLINK_MS        500U

int main(void)
{
    gpio_config_t pb5 = {
        .pin        = {GPIOB, GPIO_PIN_5},
        .mode       = GPIO_MODE_OUTPUT,
        .init_state = GPIO_STATE_SET
    };

    systick_init(CORE_CLOCK_HZ);
    gpio_init(&pb5);

    while(1)
    {
        gpio_toggle(pb5.pin);
        systick_delay_ms(BLINK_MS);
    }
}