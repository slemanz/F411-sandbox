#include "bsp/led.h"
#include "core/uprint.h"
#include "shared/pool.h"

struct led_t{
    const char *name;
    IO_Interface_t *pin;
    uint32_t uuid;
    ledPtr_t next;
};

ledPtr_t led_header = NULL;
uint32_t uuid_count = 0;

static void ledList_insert(ledPtr_t led);
static void ledList_delete(ledPtr_t led);

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin)
{
    ledPtr_t led = (ledPtr_t)pool_Allocate();

    if(led)
    {
        led->name = name;
        led->pin = io_pin;
        led->uuid = ++uuid_count;
        
        if(led_header == NULL)

        uprint("*** %s created ***\r\n", led->name);
    }else
    {
        uprint("Low memory, cannot create device\r\n");
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
    uprint("*** %s destroyed***\r\n", led->name);
    pool_free(led);
}

void led_displayInfo(ledPtr_t led)
{
    uprint("************************************************************\r\n");
    uprint("Device name: %s\r\n", led->name);
    uprint("************************************************************\r\n");
}

void led_displayAll(void)
{
    uprint("************************************************************\r\n");
    //uprint("Device name: %s\r\n", led->name);
    uprint("************************************************************\r\n");
}

static void ledList_insert(ledPtr_t led)
{

}

static void ledList_delete(ledPtr_t led)
{

}