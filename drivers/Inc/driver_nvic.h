#ifndef INC_DRIVER_NVIC_H_
#define INC_DRIVER_NVIC_H_

#include <stdint.h>
#include "stm32f4xx.h"

/********************************************************************************************
 *                              APIs supported by this driver
 *                  for more information check the function definitions
 ********************************************************************************************/

/*
 * Enable and Disable
 */

void nvic_enable(IRQn_Type irq);
void nvic_disable(IRQn_Type irq);

/*
 * Priority (0 = highest, 15 = lowest)
 */

void nvic_set_priority(IRQn_Type irq, uint8_t priority);

#endif /* INC_DRIVER_NVIC_H_ */
