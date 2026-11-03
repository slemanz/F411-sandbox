#include "driver_systick.h"

static volatile uint32_t s_ticks_ms = 0;

/*
 * Init and De-init
 */

void systick_init(uint32_t core_clock_hz)
{
    s_ticks_ms = 0;

    SysTick->LOAD = (core_clock_hz/1000U) - 1U;     /* 1ms period */
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    /* core clock */
                    SysTick_CTRL_TICKINT_Msk   |    /* interrupt on each tick */
                    SysTick_CTRL_ENABLE_Msk;        /* start counter */
}

void systick_deinit(void)
{

}

uint32_t systick_get_ms(void)
{
    return 0;
}

void systick_delay_ms(uint32_t ms)
{

}

void systick_timeout_start(systick_timeout_t *timeout, uint32_t duration_ms)
{

}

bool systick_timeout_expired(const systick_timeout_t *timeout)
{
    return false;
}
