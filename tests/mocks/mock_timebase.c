#include "mock_timebase.h"

static uint64_t fake_ticks = 0;

static uint64_t mock_get(void)
{
    return fake_ticks;
}

static void mock_deinit(void)
{
    /* Nothing to deinitialize in mock */
}

static timebase_interface_t mock_timebase = {
    .get = mock_get,
    .deinit = mock_deinit
};

/* ------------------------------------------------------------------ */
/*  Control functions called from test cases                          */
/* ------------------------------------------------------------------ */

void mock_timebase_set_ticks(uint64_t ticks)
{
    fake_ticks = ticks;
}

void mock_timebase_advance(uint64_t delta)
{
    fake_ticks += delta;
}

void mock_timebase_reset(void)
{
    fake_ticks = 0;
}

/* ------------------------------------------------------------------ */
/*  Replaces the real timebase_get() at link time                      */
/* ------------------------------------------------------------------ */

timebase_interface_t *timebase_get(void)
{
    return &mock_timebase;
}