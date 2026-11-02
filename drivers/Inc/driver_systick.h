#ifndef INC_DRIVER_SYSTICK_H_
#define INC_DRIVER_SYSTICK_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

/**
 * @brief Timeout started by systick_timeout_start
 */
typedef struct
{
    uint32_t start_ms;
    uint32_t duration_ms;
}systick_timeout_t;

/********************************************************************************************
 *                              APIs supported by this driver
 *                  for more information check the function definitions
 ********************************************************************************************/

/*
 * Init and De-init
 */

void systick_init(uint32_t core_clock_hz);
void systick_deinit(void);

/*
 * Time
 */

uint32_t systick_get_ms(void);
void systick_delay_ms(uint32_t ms);

/*
 * Timeout (non-blocking)
 */

void systick_timeout_start(systick_timeout_t *timeout, uint32_t duration_ms);
bool systick_timeout_expired(const systick_timeout_t *timeout);

#endif /* INC_DRIVER_SYSTICK_H_ */