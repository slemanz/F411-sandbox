#include "unity_fixture.h"
#include "core/simple-timer.h"
#include "mock_timebase.h"

/* ================================================================== */
/*  Test Group                                                         */
/* ================================================================== */

TEST_GROUP(SimpleTimer);

TEST_SETUP(SimpleTimer)
{
    mock_timebase_reset();
}

TEST_TEAR_DOWN(SimpleTimer)
{
    /* Nothing to clean up */
}

/* ================================================================== */
/*  Helpers                                                            */
/* ================================================================== */

/** Convenience: create a one-shot timer starting at tick 0. */
static simple_timer_t make_oneshot(uint64_t wait_ms)
{
    simple_timer_t t;
    simple_timer_setup(&t, wait_ms, false);
    return t;
}

/** Convenience: create an auto-reset timer starting at tick 0. */
static simple_timer_t make_autoreset(uint64_t wait_ms)
{
    simple_timer_t t;
    simple_timer_setup(&t, wait_ms, true);
    return t;
}

/* ================================================================== */
/*  Tests — One-shot behaviour                                         */
/* ================================================================== */

TEST(SimpleTimer, DoesNotElapseBforeDeadline)
{
    simple_timer_t t = make_oneshot(100);

    mock_timebase_advance(99);
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t));
}


/* ================================================================== */
/*  Test runner                                                        */
/* ================================================================== */

TEST_GROUP_RUNNER(SimpleTimer)
{
    /* One-shot */
    RUN_TEST_CASE(SimpleTimer, DoesNotElapseBforeDeadline);
}