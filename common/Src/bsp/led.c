#include "bsp/led.h"
#include "core/uprint.h"
#include "shared/pool.h"

struct led_t{
    const char *name;
    IO_Interface_t *pin;
    uint32_t uuid;
    bool inverted;
    ledPtr_t next;
};

ledPtr_t led_header = NULL;
uint32_t uuid_count = 1;

static void ledList_insert(ledPtr_t led);
static void ledList_delete(ledPtr_t led);
static bool ledList_uuidExists(uint32_t uuid);


/************************************************************
*                       CREATE                              *
*************************************************************/

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin)
{
    ledPtr_t led = (ledPtr_t)pool_Allocate();

    if(led)
    {
        led->name = name;
        led->pin = io_pin;
        led->inverted = false;
        led->next = NULL;

        while(ledList_uuidExists(uuid_count) == true)
        {
            uuid_count++;
        }
        led->uuid = uuid_count++;
        
        ledList_insert(led);

        uprint("*** %s created ***\r\n", led->name);
    }else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return led;
}

void led_destroy(ledPtr_t led)
{
    uprint("*** %s destroyed***\r\n", led->name);
    ledList_delete(led);
    pool_Free(led);
}

ledPtr_t led_createWithUuid(const char *name, IO_Interface_t *io_pin, uint32_t uuid)
{
    if(ledList_uuidExists(uuid))
    {
        uprint("Failed to create %s: UUID %lu already exists\r\n", name, uuid);
        return NULL;
    }

    ledPtr_t led = (ledPtr_t)pool_Allocate();

    if(led)
    {
        led->name = name;
        led->pin = io_pin;
        led->uuid = uuid;
        led->inverted = false;
        led->next = NULL;

        ledList_insert(led);
        uprint("*** %s created with UUID %d ***\r\n", led->name, led->uuid);
    }else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return led;
}

ledPtr_t led_getByUuid(uint32_t uuid)
{
    ledPtr_t current = led_header;

    while(current != NULL)
    {
        if(current->uuid == uuid)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


/************************************************************
*                       CONTROL                             *
*************************************************************/

void led_turn_on(ledPtr_t led)
{
    uint8_t state = 1;
    if(led->inverted)
    {
        state = 0;
    }
    led->pin->write(state);
}

void led_turn_off(ledPtr_t led)
{
    uint8_t state = 0;
    if(led->inverted)
    {
        state = 1;
    }
    led->pin->write(state);
}

void led_toggle(ledPtr_t led)
{
    led->pin->toggle();
}

void led_invertLogic(ledPtr_t led)
{
    if(led == NULL)
    {
        return;
    }
    led->inverted = true;
}

/************************************************************
*                       DISPLAY                             *
*************************************************************/

void led_displayInfo(ledPtr_t led)
{
    uprint("************************************************************\r\n");
    uprint("Device name: %s\r\n", led->name);
    uprint("************************************************************\r\n");
}

void led_displayAll(void)
{
    ledPtr_t current = led_header;
    uprint("************************************************************\r\n");
    while(current != NULL)
    {
        uprint("Device name: %s (UUID: %d)\r\n", current->name, current->uuid);
        current = current->next;
    }
    uprint("************************************************************\r\n");
}

/************************************************************
*                          LIST                             *
*************************************************************/

static void ledList_insert(ledPtr_t led)
{
    if(led_header == NULL)
    {
        led_header = led;
        led->next = NULL;
    }else
    {
        ledPtr_t current = led_header;

        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = led;
        led->next = NULL;
    }
}

static void ledList_delete(ledPtr_t led)
{
    ledPtr_t current = led_header;
    ledPtr_t previous = NULL;

    while(current != NULL)
    {
        if(current == led)
        {
            if(previous == NULL)
            {
                led_header = current->next;
            }else
            {
                previous->next = current->next;
            }
            return;
        }

        previous = current;
        current = current->next;
    }
}

static bool ledList_uuidExists(uint32_t uuid)
{
    ledPtr_t current = led_header;
    
    while(current != NULL)
    {
        if(current->uuid == uuid)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}