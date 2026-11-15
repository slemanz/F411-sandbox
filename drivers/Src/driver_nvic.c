#include "driver_nvic.h"

/*
 * Enable and Disable
 */

void nvic_enable(IRQn_Type irq)
{
    uint32_t n = (uint32_t)irq;

    NVIC->ISER[n / 32U] = 1U << (n % 32U);
}

void nvic_disable(IRQn_Type irq)
{
    uint32_t n = (uint32_t)irq;

    NVIC->ICER[n / 32U] = 1U << (n % 32U);
    __DSB();
    __ISB();
}

/*
 * Priority
 */

void nvic_set_priority(IRQn_Type irq, uint8_t priority)
{
    NVIC->IPR[(uint32_t)irq] = (uint8_t)(priority << (8U - __NVIC_PRIO_BITS));
}