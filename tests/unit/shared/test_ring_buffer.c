#include "unity_fixture.h"
#include "shared/ring-buffer.h"

TEST_GROUP(RingBuffer);

/*
 * ring_buffer_setup() requires that size is a power of two because the
 * implementation uses a bitmask. All test buffers follow this rule.
 */

#define BUF_SIZE    8   /* power of two */

static ring_buffer_t rb;
static uint8_t       backing[BUF_SIZE];

TEST_SETUP(RingBuffer)
{
    ring_buffer_setup(&rb, backing, BUF_SIZE);
}

TEST_TEAR_DOWN(RingBuffer)
{
}

/* ================================================================== */
/*  Tests — initial state                                             */
/* ================================================================== */

TEST(RingBuffer, IsEmptyAfterSetup)
{
    TEST_ASSERT_TRUE(ring_buffer_empty(&rb));
}

TEST(RingBuffer, ReadFromEmptyReturnsFalse)
{
    uint8_t byte;
    TEST_ASSERT_FALSE(ring_buffer_read(&rb, &byte));
}

/* ================================================================== */
/*  Tests — write / read                                              */
/* ================================================================== */


/* ================================================================== */
/*  Test runner                                                       */
/* ================================================================== */

TEST_GROUP_RUNNER(RingBuffer)
{
    /* Initial State */
    RUN_TEST_CASE(RingBuffer, IsEmptyAfterSetup);
    RUN_TEST_CASE(RingBuffer, ReadFromEmptyReturnsFalse);

    /* Write / Read */
}