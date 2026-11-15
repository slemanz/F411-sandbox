#include "driver_gpio.h"
#include "driver_systick.h"
#include "driver_uart.h"

#define CORE_CLOCK_HZ   16000000U
#define BLINK_MS        500U
#define HELLO_MS        10000U

static const gpio_config_t uart_pins[] = {
    { .pin = { GPIOA, GPIO_PIN_2 }, .mode = GPIO_MODE_ALTFN, .af = GPIO_AF7 },
    { .pin = { GPIOA, GPIO_PIN_3 }, .mode = GPIO_MODE_ALTFN, .af = GPIO_AF7, .pull = GPIO_PULL_UP },
};

static const uart_config_t uart = {
    .instance = USART2,
    .clock_hz = CORE_CLOCK_HZ,
    .baudrate = 115200,
};

static const gpio_config_t led = {
    .pin        = { GPIOB, GPIO_PIN_5 }, .mode       = GPIO_MODE_OUTPUT, .init_state = GPIO_STATE_SET,
};


int main(void)
{
    systick_init(CORE_CLOCK_HZ);
    gpio_init(&led);
    gpio_init_table(uart_pins, 2);
    uart_init(&uart);

    systick_timeout_t blink;
    systick_timeout_t hello;
    systick_timeout_start(&blink, BLINK_MS);
    systick_timeout_start(&hello, HELLO_MS);

    uart_write_string(uart.instance, "Hello World!\r\n");

    while(1)
    {
        if(systick_timeout_expired(&blink))
        {
            systick_timeout_start(&blink, BLINK_MS);
            gpio_toggle(led.pin);
        }

        if(systick_timeout_expired(&hello))
        {
            systick_timeout_start(&hello, HELLO_MS);
            uart_write_string(uart.instance, "Hello World!\r\n");
        }
    }
}