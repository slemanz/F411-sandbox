#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>
#include "interface/interface.h"

typedef struct led_t* ledPtr_t;

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin);

void led_turn_on(ledPtr_t led);
void led_turn_off(ledPtr_t led);
void led_toggle(ledPtr_t led);

void led_destroy(ledPtr_t led);
void led_displayInfo(ledPtr_t led);


#endif /* INC_LED_H_ */