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

TEST(RingBuffer, ReadReturnsTrueWhenNotFull)
{
    TEST_ASSERT_TRUE(ring_buffer_write(&rb, 0xAB));
}

TEST(RingBuffer, ReadReturnsByteWritten)
{
    ring_buffer_write(&rb, 0x42);

    uint8_t byte = 0;
    TEST_ASSERT_TRUE(ring_buffer_read(&rb, &byte));
    TEST_ASSERT_EQUAL_UINT8(0x42, byte);
}

TEST(RingBuffer, NotEmptyAfterWrite)
{
    ring_buffer_write(&rb, 0x01);
    TEST_ASSERT_FALSE(ring_buffer_empty(&rb));
}

TEST(RingBuffer, EmptyAfterWriteThenRead)
{
    uint8_t byte;
    ring_buffer_write(&rb, 0x55);
    ring_buffer_read(&rb, &byte);
    TEST_ASSERT_TRUE(ring_buffer_empty(&rb));
}


TEST(RingBuffer, PreservesFIFOOrder)
{
    /*
     * Write bytes 1, 2, 3 and verify they come back in the same order
     */
    ring_buffer_write(&rb, 1);
    ring_buffer_write(&rb, 2);
    ring_buffer_write(&rb, 3);

    uint8_t a, b, c;
    ring_buffer_read(&rb, &a);
    ring_buffer_read(&rb, &b);
    ring_buffer_read(&rb, &c);

    TEST_ASSERT_EQUAL_UINT8(1, a);
    TEST_ASSERT_EQUAL_UINT8(2, b);
    TEST_ASSERT_EQUAL_UINT8(3, c);
}

/* ================================================================== */
/*  Tests — capacity                                                  */
/* ================================================================== */

TEST(RingBuffer, FillsToCapacityMinusOne)
{
    /*
     * The implementation sacrifices one slot to distinguish full from
     * empty, so a buffer of size N holds N-1 bytes.
     */
    uint8_t capacity = BUF_SIZE - 1;

    for(uint8_t i = 0; i < capacity; i++)
    {
        TEST_ASSERT_TRUE(ring_buffer_write(&rb, i));
    }
}

TEST(RingBuffer, WriteReturnsFalseWhenFull)
{
    uint8_t capacity = BUF_SIZE - 1;

    for(uint8_t i = 0; i < capacity; i++)
    {
        ring_buffer_write(&rb, i);
    }
    /* One more write must be rejected */
    TEST_ASSERT_FALSE(ring_buffer_write(&rb, 0xFF));
}


TEST(RingBuffer, OverflowDoesNotCorruptData)
{
    /*
     * Fill the buffer, reject the overflow write, then read back all
     * bytes and verify none were corrupted.
     */
    uint8_t capacity = BUF_SIZE - 1;

    for(uint8_t i = 0; i < capacity; i++)
    {
        ring_buffer_write(&rb, i);
    }

    ring_buffer_write(&rb, 0xFF); /* Rejected */

    for(uint8_t i = 0; i < capacity; i++)
    {
        uint8_t byte;
        ring_buffer_read(&rb, &byte);
        TEST_ASSERT_EQUAL_UINT8(i, byte);
    }
}

/* ================================================================== */
/*  Tests — wrap-around                                               */
/* ================================================================== */

TEST(RingBuffer, HandlesWrapAround)
{
    /*
     * Fill half the buffer, drain it, fill again — this forces the
     * write and read indices to wrap around the backing array.
     */
    uint8_t half = BUF_SIZE/2;
    uint8_t byte;

    // first half
    for(uint8_t i = 0; i < half; i++) ring_buffer_write(&rb, i);
    for(uint8_t i = 0; i < half; i++) ring_buffer_read(&rb, &byte);

    // second half
    for(uint8_t i = 0; i < half; i++) ring_buffer_write(&rb, (uint8_t)(i + 0x10));


    for (uint8_t i = 0; i < half; i++)
    {
        ring_buffer_read(&rb, &byte);
        TEST_ASSERT_EQUAL_UINT8((uint8_t)(i + 0x10), byte);
    }
}

TEST(RingBuffer, EmptyAfterFullCycleWithWrapAround)
{
    uint8_t capacity = BUF_SIZE - 1;
    uint8_t byte;

    for (uint8_t i = 0; i < capacity; i++) ring_buffer_write(&rb, i);
    for (uint8_t i = 0; i < capacity; i++) ring_buffer_read(&rb, &byte);

    TEST_ASSERT_TRUE(ring_buffer_empty(&rb));
}


/* ================================================================== */
/*  Test runner                                                       */
/* ================================================================== */

TEST_GROUP_RUNNER(RingBuffer)
{
    /* Initial State */
    RUN_TEST_CASE(RingBuffer, IsEmptyAfterSetup);
    RUN_TEST_CASE(RingBuffer, ReadFromEmptyReturnsFalse);

    /* Write / Read */
    RUN_TEST_CASE(RingBuffer, ReadReturnsTrueWhenNotFull);
    RUN_TEST_CASE(RingBuffer, ReadReturnsByteWritten);
    RUN_TEST_CASE(RingBuffer, NotEmptyAfterWrite);
    RUN_TEST_CASE(RingBuffer, EmptyAfterWriteThenRead);
    RUN_TEST_CASE(RingBuffer, PreservesFIFOOrder);

    /* Capacity */
    RUN_TEST_CASE(RingBuffer, FillsToCapacityMinusOne);
    RUN_TEST_CASE(RingBuffer, WriteReturnsFalseWhenFull);
    RUN_TEST_CASE(RingBuffer, OverflowDoesNotCorruptData);

    /* Wrap-around*/
    RUN_TEST_CASE(RingBuffer, HandlesWrapAround);
    RUN_TEST_CASE(RingBuffer, EmptyAfterFullCycleWithWrapAround);
}