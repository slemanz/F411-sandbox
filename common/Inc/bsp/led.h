#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>
#include <stdbool.h>
#include "interface/interface.h"

typedef struct led_t* ledPtr_t;

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin);
ledPtr_t led_createWithUuid(const char *name, IO_Interface_t *io_pin, uint32_t uuid);
ledPtr_t led_getByUuid(uint32_t uuid);
void led_destroy(ledPtr_t led);

void led_turn_on(ledPtr_t led);
void led_turn_off(ledPtr_t led);
void led_toggle(ledPtr_t led);

void led_invertLogic(ledPtr_t led);

void led_displayInfo(ledPtr_t led);
void led_displayAll(void);


#endif /* INC_LED_H_ */