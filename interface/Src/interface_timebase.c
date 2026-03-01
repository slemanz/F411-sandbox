#include "interface/interface.h"
#include "driver_systick.h"

static uint8_t s_is_init = 0u;

static void timebase_ensure_init(void)
{
    if (!s_is_init)
    {
        systick_init(1000u);
        s_is_init = 1u;
    }
}

uint64_t timebase_get(void)
{
    timebase_ensure_init();
    return ticks_get();
}

void timebase_deinit(void)
{
    // todo
    s_is_init = 0u;
}