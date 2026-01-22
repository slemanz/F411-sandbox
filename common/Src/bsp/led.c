#include "bsp/led.h"
#include "core/uprint.h"
#include "shared/pool.h"

struct led_t{
    const char *name;
    IO_Interface_t *pin;
};

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin)
{
    ledPtr_t led = (ledPtr_t)pool_Allocate();

    if(led)
    {
        led->name = name;
        led->pin = io_pin;
    }else
    {
        uprint("Low memory, cannot create device\n");
    }

    return led;
}

void led_turn_on(ledPtr_t led)
{
    led->pin->write(1);
}

void led_turn_off(ledPtr_t led)
{
    led->pin->write(0);
}

void led_toggle(ledPtr_t led)
{
    led->pin->toggle();
}

void led_destroy(ledPtr_t led)
{
    uprint("*** %s destroyed***\n\r", led->name);
    pool_free(led);
}

void led_displayInfo(ledPtr_t led)
{

}