#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>
#include <stdbool.h>
#include "interface/interface.h"

/************************************************************
*                          LED                              *
*************************************************************/

typedef struct led_t* ledPtr_t;

/************************************************************
*                    CREATE/DESTROY                         *
*************************************************************/

ledPtr_t led_create(const char *name, IO_Interface_t *io_pin);
ledPtr_t led_createWithUuid(const char *name, IO_Interface_t *io_pin, uint32_t uuid);
ledPtr_t led_getByUuid(uint32_t uuid);
void led_destroy(ledPtr_t led);

/************************************************************
*                     CONFIGURATION                         *
*************************************************************/

void led_invertLogic(ledPtr_t led);

/************************************************************
*                        CONTROL                            *
*************************************************************/

void led_turn_on(ledPtr_t led);
void led_turn_off(ledPtr_t led);
void led_toggle(ledPtr_t led);

/************************************************************
*                        DISPLAY                            *
*************************************************************/

void led_displayInfo(ledPtr_t led);
void led_displayAll(void);

/************************************************************
*                      LED RGB                              *
*************************************************************/

typedef struct led_rgb_t *ledRgbPtr_t;

typedef enum
{
    LED_RGB_COLOR_OFF     = 0,
    LED_RGB_COLOR_RED,
    LED_RGB_COLOR_GREEN,
    LED_RGB_COLOR_BLUE,
    LED_RGB_COLOR_YELLOW,
    LED_RGB_COLOR_CYAN,
    LED_RGB_COLOR_MAGENTA,
    LED_RGB_COLOR_WHITE,
} led_rgb_color_e;

/************************************************************
*                    CREATE/DESTROY                         *
*************************************************************/

ledRgbPtr_t led_rgb_create(const char *name,
                            IO_Interface_t *pin_r,
                            IO_Interface_t *pin_g,
                            IO_Interface_t *pin_b);

ledRgbPtr_t led_rgb_createWithUuid(const char *name,
                                    IO_Interface_t *pin_r,
                                    IO_Interface_t *pin_g,
                                    IO_Interface_t *pin_b,
                                    uint8_t uuid);

void        led_rgb_destroy(ledRgbPtr_t led);

ledRgbPtr_t led_rgb_getByUuid(uint8_t uuid);

/************************************************************
*                     CONFIGURATION                         *
*************************************************************/

void led_rgb_invertLogic(ledRgbPtr_t led);

/************************************************************
*                        CONTROL                            *
*************************************************************/

void led_rgb_set(ledRgbPtr_t led, led_rgb_color_e color);
void led_rgb_setRaw(ledRgbPtr_t led, bool r, bool g, bool b);
void led_rgb_off(ledRgbPtr_t led);

led_rgb_color_e led_rgb_getColor(ledRgbPtr_t led);
void led_rgb_toggle(ledRgbPtr_t led, led_rgb_color_e color);

/************************************************************
*                        DISPLAY                            *
*************************************************************/

void led_rgb_displayInfo(ledRgbPtr_t led);
void led_rgb_displayAll(void);

#endif /* INC_LED_H_ */