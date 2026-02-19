#include "bsp/button.h"
#include "core/uprint.h"
#include "core/simple-timer.h"
#include "shared/pool.h"

/*******************************************************************************
 * Private types
 ******************************************************************************/

typedef enum
{
    BTN_STATE_IDLE = 0,   /* Pin is inactive (not pressed)                   */
    BTN_STATE_DEBOUNCE,   /* Pin went active — waiting for debounce window   */
    BTN_STATE_PRESSED,    /* Debounced press confirmed                        */
    BTN_STATE_HELD,       /* Hold timer elapsed while still pressed           */
    BTN_STATE_RELEASE_DB, /* Pin went inactive — waiting for release debounce */
} btn_state_e;

struct button_t
{
    const char     *name;
    IO_Interface_t *pin;
    uint32_t        uuid;
    bool            inverted;

    uint32_t        debounce_ms;
    uint32_t        hold_time_ms;

    btn_state_e     state;
    simple_timer_t  debounce_timer;
    simple_timer_t  hold_timer;

    button_event_e  pending_event;  /* Consumed by button_getEvent()         */
    bool            is_pressed;     /* Debounced logical state               */

    buttonPtr_t     next;
};

static buttonPtr_t btn_header  = NULL;
static uint32_t    uuid_count  = 1;

static void btnList_insert(buttonPtr_t btn);
static void btnList_delete(buttonPtr_t btn);
static bool btnList_uuidExists(uint32_t uuid);

/*******************************************************************************
 * Helpers
 ******************************************************************************/

static bool read_pin(buttonPtr_t btn)
{
    uint8_t raw = btn->pin->read();
    return btn->inverted ? (raw == 0) : (raw != 0);
}
/************************************************************
*                       CREATE                              *
*************************************************************/

buttonPtr_t button_create(const char *name,
                           IO_Interface_t *pin,
                           uint32_t debounce_ms,
                           uint32_t hold_time_ms)
{
    buttonPtr_t btn = (buttonPtr_t)poolBig_Allocate();

    if(btn)
    {
        btn->name          = name;
        btn->pin           = pin;
        btn->inverted      = false;
        btn->debounce_ms   = debounce_ms;
        btn->hold_time_ms  = hold_time_ms;
        btn->state         = BTN_STATE_IDLE;
        btn->pending_event = BUTTON_EVENT_NONE;
        btn->is_pressed    = false;
        btn->next          = NULL;

        while(btnList_uuidExists(uuid_count) == true)
        {
            uuid_count++;
        }
        btn->uuid = uuid_count++;

        btnList_insert(btn);
        uprint("*** %s created ***\r\n", btn->name);
    }
    else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return btn;
}

buttonPtr_t button_createWithUuid(const char *name,
                                   IO_Interface_t *pin,
                                   uint32_t debounce_ms,
                                   uint32_t hold_time_ms,
                                   uint32_t uuid)
{
    if(btnList_uuidExists(uuid))
    {
        uprint("Failed to create %s: UUID %lu already exists\r\n", name, uuid);
        return NULL;
    }

    buttonPtr_t btn = (buttonPtr_t)poolBig_Allocate();

    if(btn)
    {
        btn->name          = name;
        btn->pin           = pin;
        btn->uuid          = uuid;
        btn->inverted      = false;
        btn->debounce_ms   = debounce_ms;
        btn->hold_time_ms  = hold_time_ms;
        btn->state         = BTN_STATE_IDLE;
        btn->pending_event = BUTTON_EVENT_NONE;
        btn->is_pressed    = false;
        btn->next          = NULL;

        btnList_insert(btn);
        uprint("*** %s created with UUID %d ***\r\n", btn->name, btn->uuid);
    }
    else
    {
        uprint("Low memory, cannot create device\r\n");
    }

    return btn;
}

void button_destroy(buttonPtr_t btn)
{
    uprint("*** %s destroyed ***\r\n", btn->name);
    btnList_delete(btn);
    poolBig_Free(btn);
}

buttonPtr_t button_getByUuid(uint32_t uuid)
{
    buttonPtr_t current = btn_header;

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
*                    CONFIGURATION                          *
*************************************************************/

void button_invertLogic(buttonPtr_t btn)
{
    if(btn == NULL) return;
    btn->inverted = true;
}

/************************************************************
*                    UPDATE/EVENTS                          *
*************************************************************/

void button_update(buttonPtr_t btn)
{
    if(btn == NULL) return;

    bool active = read_pin(btn);

    switch(btn->state)
    {
        case BTN_STATE_IDLE:
        {
            if(active)
            {
                simple_timer_setup(&btn->debounce_timer, btn->debounce_ms, false);
                btn->state = BTN_STATE_DEBOUNCE;
            }
            break;
        }

        case BTN_STATE_DEBOUNCE:
        {
            if(!active)
            {
                /* Glitch — bail out before debounce window closes */
                btn->state = BTN_STATE_IDLE;
            }
            else if(simple_timer_has_elapsed(&btn->debounce_timer))
            {
                btn->state         = BTN_STATE_PRESSED;
                btn->is_pressed    = true;
                btn->pending_event = BUTTON_EVENT_PRESSED;

                if(btn->hold_time_ms > 0)
                {
                    simple_timer_setup(&btn->hold_timer, btn->hold_time_ms, false);
                }
            }
            break;
        }

        case BTN_STATE_PRESSED:
        {
            if(!active)
            {
                simple_timer_setup(&btn->debounce_timer, btn->debounce_ms, false);
                btn->state = BTN_STATE_RELEASE_DB;
            }
            else if(btn->hold_time_ms > 0 &&
                    simple_timer_has_elapsed(&btn->hold_timer))
            {
                btn->state         = BTN_STATE_HELD;
                btn->pending_event = BUTTON_EVENT_HELD;
            }
            break;
        }

        case BTN_STATE_HELD:
        {
            if(!active)
            {
                simple_timer_setup(&btn->debounce_timer, btn->debounce_ms, false);
                btn->state = BTN_STATE_RELEASE_DB;
            }
            break;
        }

        case BTN_STATE_RELEASE_DB:
        {
            if(active)
            {
                /* Glitch on release — go back to pressed */
                btn->state = BTN_STATE_PRESSED;
            }
            else if(simple_timer_has_elapsed(&btn->debounce_timer))
            {
                btn->state         = BTN_STATE_IDLE;
                btn->is_pressed    = false;
                btn->pending_event = BUTTON_EVENT_RELEASED;
            }
            break;
        }

        default: break;
    }
}

button_event_e button_getEvent(buttonPtr_t btn)
{
    if(btn == NULL) return BUTTON_EVENT_NONE;

    button_event_e event = btn->pending_event;
    btn->pending_event   = BUTTON_EVENT_NONE;
    return event;
}

bool button_isPressed(buttonPtr_t btn)
{
    if(btn == NULL) return false;
    return btn->is_pressed;
}

/************************************************************
*                       DISPLAY                             *
*************************************************************/

void button_displayInfo(buttonPtr_t btn)
{
    uprint("************************************************************\r\n");
    uprint("Device name: %s (UUID: %d)\r\n", btn->name, btn->uuid);
    uprint("************************************************************\r\n");
}

void button_displayAll(void)
{
    buttonPtr_t current = btn_header;
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

static void btnList_insert(buttonPtr_t btn)
{
    if(btn_header == NULL)
    {
        btn_header = btn;
        btn->next  = NULL;
    }
    else
    {
        buttonPtr_t current = btn_header;

        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = btn;
        btn->next     = NULL;
    }
}

static void btnList_delete(buttonPtr_t btn)
{
    buttonPtr_t current  = btn_header;
    buttonPtr_t previous = NULL;

    while(current != NULL)
    {
        if(current == btn)
        {
            if(previous == NULL)
            {
                btn_header = current->next;
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

static bool btnList_uuidExists(uint32_t uuid)
{
    buttonPtr_t current = btn_header;

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
