#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>
#include <stdbool.h>
#include "interface/interface.h"

typedef struct button_t *buttonPtr_t;

typedef enum
{
    BUTTON_EVENT_NONE     = 0,
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED,
    BUTTON_EVENT_HELD,
} button_event_e;

/************************************************************
*                    CREATE/DESTROY                         *
*************************************************************/

buttonPtr_t button_create(const char *name,
                           IO_Interface_t *pin,
                           uint32_t debounce_ms,
                           uint32_t hold_time_ms);

buttonPtr_t button_createWithUuid(const char *name,
                                   IO_Interface_t *pin,
                                   uint32_t debounce_ms,
                                   uint32_t hold_time_ms,
                                   uint32_t uuid);

void        button_destroy(buttonPtr_t btn);

buttonPtr_t button_getByUuid(uint32_t uuid);

/************************************************************
*                     CONFIGURATION                         *
*************************************************************/

void button_invertLogic(buttonPtr_t btn);

/************************************************************
*                      UPDATE/EVENTS                        *
*************************************************************/

void button_update(buttonPtr_t btn);
button_event_e button_getEvent(buttonPtr_t btn);
bool button_isPressed(buttonPtr_t btn);

/************************************************************
*                        DISPLAY                            *
*************************************************************/

void button_displayInfo(buttonPtr_t btn);
void button_displayAll(void);


#endif /* INC_BUTTON_H_ */