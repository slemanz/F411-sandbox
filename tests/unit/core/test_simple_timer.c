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

TEST(SimpleTimer, ElapsedExactlyAtDeadLine)
{
    simple_timer_t t = make_oneshot(100);

    mock_timebase_advance(100);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

TEST(SimpleTimer, ElapsesWhenPastDeadline)
{
    simple_timer_t t = make_oneshot(100);

    mock_timebase_advance(200);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

TEST(SimpleTimer, OneshotFiresOnlyOnce)
{
    simple_timer_t t = make_oneshot(100);

    mock_timebase_advance(100);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));  // first call
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t)); // second call
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t)); // third call
}

TEST(SimpleTimer, OneshotResetAllowsNewFire)
{
    simple_timer_t t = make_oneshot(100);

    mock_timebase_advance(100);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));

    /* reset from current tick */
    simple_timer_reset(&t);
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(100);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

/* ================================================================== */
/*  Tests — Auto-reset behaviour                                      */
/* ================================================================== */

TEST(SimpleTimer, AutoResetFiresRepeatedly)
{
    simple_timer_t t = make_autoreset(50);

    mock_timebase_advance(50);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(50);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(50);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

TEST(SimpleTimer, AutoResetDoesNotFireBetweenPeriods)
{
    simple_timer_t t = make_autoreset(50);

    mock_timebase_advance(50);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(25);
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t));
}

TEST(SimpleTimer, AutoResetCompensatesDrift)
{
    /*
     * Period = 100 ms, ideal deadlines at 100, 200, 300...
     * If we poll at 110 ms (10 ms late), the next deadline 
     * must remain exactly at 200 ms to stay accurate.
     */
    simple_timer_t t = make_autoreset(100);

    mock_timebase_advance(110);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(80);
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(10);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

/* ================================================================== */
/*  Tests — setup edge cases                                           */
/* ================================================================== */

TEST(SimpleTimer, ZeroWaitTimeElapsesImmediately)
{
    simple_timer_t t = make_oneshot(0);

    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}

TEST(SimpleTimer, TimerStartedInTheFutureDoesNotElapse)
{
    mock_timebase_set_ticks(1000);

    simple_timer_t t;
    simple_timer_setup(&t, 500, false);

    mock_timebase_advance(499);
    TEST_ASSERT_FALSE(simple_timer_has_elapsed(&t));

    mock_timebase_advance(1);
    TEST_ASSERT_TRUE(simple_timer_has_elapsed(&t));
}


/* ================================================================== */
/*  Test runner                                                        */
/* ================================================================== */

TEST_GROUP_RUNNER(SimpleTimer)
{
    /* One-shot */
    RUN_TEST_CASE(SimpleTimer, DoesNotElapseBforeDeadline);
    RUN_TEST_CASE(SimpleTimer, ElapsedExactlyAtDeadLine);
    RUN_TEST_CASE(SimpleTimer, ElapsesWhenPastDeadline);
    RUN_TEST_CASE(SimpleTimer, OneshotFiresOnlyOnce);
    RUN_TEST_CASE(SimpleTimer, OneshotResetAllowsNewFire);

    /* Auto-reset */
    RUN_TEST_CASE(SimpleTimer, AutoResetFiresRepeatedly);
    RUN_TEST_CASE(SimpleTimer, AutoResetDoesNotFireBetweenPeriods);
    RUN_TEST_CASE(SimpleTimer, AutoResetCompensatesDrift);

    /* Edge cases */
    RUN_TEST_CASE(SimpleTimer, ZeroWaitTimeElapsesImmediately);
    RUN_TEST_CASE(SimpleTimer, TimerStartedInTheFutureDoesNotElapse);
}