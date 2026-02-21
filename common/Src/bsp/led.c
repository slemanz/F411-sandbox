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
    if(led == NULL) return;

    uprint("*** %s destroyed ***\r\n", led->name);
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
    if(led == NULL) return;

    uint8_t state = led->inverted ? 0 : 1;
    led->pin->write(state);
}

void led_turn_off(ledPtr_t led)
{
    if(led == NULL) return;

    uint8_t state = led->inverted ? 1 : 0;
    led->pin->write(state);
}

void led_toggle(ledPtr_t led)
{
    if(led == NULL) return;

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
    if(led == NULL) return;

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

/************************************************************
*                      LED RGB                              *
*************************************************************/

struct led_rgb_t
{
    const char      *name;
    IO_Interface_t  *pin_r;
    IO_Interface_t  *pin_g;
    IO_Interface_t  *pin_b;
    uint8_t          uuid;
    bool             inverted;
    led_rgb_color_e  current_color;  /* Last color set — used by toggle */
    ledRgbPtr_t      next;
};

static ledRgbPtr_t led_rgb_header = NULL;
static uint8_t    uuid_rgb_count     = 1;

static void ledRgbList_insert(ledRgbPtr_t led);
static void ledRgbList_delete(ledRgbPtr_t led);
static bool ledRgbList_uuidExists(uint8_t uuid);

/************************************************************
*                        HELPERS                            *
*************************************************************/

static void write_pin(IO_Interface_t *pin, bool state, bool inverted)
{
    if(pin == NULL) return;

    uint8_t val = (state ^ inverted) ? 1 : 0;
    pin->write(val);
}

/************************************************************
*                    CREATE/DESTROY                         *
*************************************************************/

ledRgbPtr_t led_rgb_create(const char *name,
                            IO_Interface_t *pin_r,
                            IO_Interface_t *pin_g,
                            IO_Interface_t *pin_b)
{
    ledRgbPtr_t led = (ledRgbPtr_t)pool_Allocate();

    if(led)
    {
        led->name     = name;
        led->pin_r    = pin_r;
        led->pin_g    = pin_g;
        led->pin_b    = pin_b;
        led->inverted = false;
        led->next     = NULL;

        while(ledRgbList_uuidExists(uuid_rgb_count) == true)
        {
            uuid_rgb_count++;
        }
        led->uuid = uuid_rgb_count++;

        ledRgbList_insert(led);

        uprint("*** %s created ***\r\n", led->name);
    }
    else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return led;
}

ledRgbPtr_t led_rgb_createWithUuid(const char *name,
                                    IO_Interface_t *pin_r,
                                    IO_Interface_t *pin_g,
                                    IO_Interface_t *pin_b,
                                    uint8_t uuid)
{
    if(ledRgbList_uuidExists(uuid))
    {
        uprint("Failed to create %s: UUID %u already exists\r\n", name, uuid);
        return NULL;
    }

    ledRgbPtr_t led = (ledRgbPtr_t)pool_Allocate();

    if(led)
    {
        led->name     = name;
        led->pin_r    = pin_r;
        led->pin_g    = pin_g;
        led->pin_b    = pin_b;
        led->uuid     = uuid;
        led->inverted = false;
        led->next     = NULL;

        ledRgbList_insert(led);
        uprint("*** %s created with UUID %d ***\r\n", led->name, led->uuid);
    }
    else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return led;
}

void led_rgb_destroy(ledRgbPtr_t led)
{
    uprint("*** %s destroyed ***\r\n", led->name);
    ledRgbList_delete(led);
    pool_Free(led);
}

ledRgbPtr_t led_rgb_getByUuid(uint8_t uuid)
{
    ledRgbPtr_t current = led_rgb_header;

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
*                     CONFIGURATION                         *
*************************************************************/

void led_rgb_invertLogic(ledRgbPtr_t led)
{
    if(led == NULL) return;
    led->inverted = true;
}

/************************************************************
*                        CONTROL                            *
*************************************************************/

void led_rgb_set(ledRgbPtr_t led, led_rgb_color_e color)
{
    if(led == NULL) return;

    /* Map color enum to R, G, B booleans */
    bool r = (color == LED_RGB_COLOR_RED    ||
              color == LED_RGB_COLOR_YELLOW ||
              color == LED_RGB_COLOR_MAGENTA||
              color == LED_RGB_COLOR_WHITE);

    bool g = (color == LED_RGB_COLOR_GREEN  ||
              color == LED_RGB_COLOR_YELLOW ||
              color == LED_RGB_COLOR_CYAN   ||
              color == LED_RGB_COLOR_WHITE);

    bool b = (color == LED_RGB_COLOR_BLUE   ||
              color == LED_RGB_COLOR_CYAN   ||
              color == LED_RGB_COLOR_MAGENTA||
              color == LED_RGB_COLOR_WHITE);

    write_pin(led->pin_r, r, led->inverted);
    write_pin(led->pin_g, g, led->inverted);
    write_pin(led->pin_b, b, led->inverted);

    led->current_color = color;
}

void led_rgb_setRaw(ledRgbPtr_t led, bool r, bool g, bool b)
{
    if(led == NULL) return;

    write_pin(led->pin_r, r, led->inverted);
    write_pin(led->pin_g, g, led->inverted);
    write_pin(led->pin_b, b, led->inverted);

    /* Best-effort color tracking for setRaw — used by toggle */
    led->current_color = LED_RGB_COLOR_OFF;
    if( r && !g && !b) led->current_color = LED_RGB_COLOR_RED;
    if(!r &&  g && !b) led->current_color = LED_RGB_COLOR_GREEN;
    if(!r && !g &&  b) led->current_color = LED_RGB_COLOR_BLUE;
    if( r &&  g && !b) led->current_color = LED_RGB_COLOR_YELLOW;
    if(!r &&  g &&  b) led->current_color = LED_RGB_COLOR_CYAN;
    if( r && !g &&  b) led->current_color = LED_RGB_COLOR_MAGENTA;
    if( r &&  g &&  b) led->current_color = LED_RGB_COLOR_WHITE;
}

void led_rgb_off(ledRgbPtr_t led)
{
    led_rgb_set(led, LED_RGB_COLOR_OFF);
}

led_rgb_color_e led_rgb_getColor(ledRgbPtr_t led)
{
    if(led == NULL) return LED_RGB_COLOR_OFF;
    return led->current_color;
}

void led_rgb_toggle(ledRgbPtr_t led, led_rgb_color_e color)
{
    if(led == NULL) return;

    if(led->current_color == LED_RGB_COLOR_OFF)
    {
        led_rgb_set(led, color);
    }
    else
    {
        led_rgb_set(led, LED_RGB_COLOR_OFF);
    }
}

/************************************************************
*                        DISPLAY                            *
*************************************************************/

void led_rgb_displayInfo(ledRgbPtr_t led)
{
    uprint("************************************************************\r\n");
    uprint("Device name: %s (UUID: %d)\r\n", led->name, led->uuid);
    uprint("************************************************************\r\n");
}

void led_rgb_displayAll(void)
{
    ledRgbPtr_t current = led_rgb_header;
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

static void ledRgbList_insert(ledRgbPtr_t led)
{
    if(led_rgb_header == NULL)
    {
        led_rgb_header = led;
        led->next = NULL;
    }
    else
    {
        ledRgbPtr_t current = led_rgb_header;

        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = led;
        led->next = NULL;
    }
}

static void ledRgbList_delete(ledRgbPtr_t led)
{
    ledRgbPtr_t current  = led_rgb_header;
    ledRgbPtr_t previous = NULL;

    while(current != NULL)
    {
        if(current == led)
        {
            if(previous == NULL)
            {
                led_rgb_header = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            return;
        }

        previous = current;
        current  = current->next;
    }
}

static bool ledRgbList_uuidExists(uint8_t uuid)
{
    ledRgbPtr_t current = led_rgb_header;

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