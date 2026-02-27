#include "unity_fixture.h"
#include "shared/pool.h"

/* ================================================================== */
/*  Test Group — Standard Pool                                        */
/* ================================================================== */

TEST_GROUP(Pool);

TEST_SETUP(Pool)
{
    pool_Init();
}

TEST_TEAR_DOWN(Pool)
{
    /* Nothing to clean up — pool_Init() in setUp resets everything */
}

/* ------------------------------------------------------------------ */
/*  Allocation                                                        */
/* ------------------------------------------------------------------ */

TEST(Pool, AllocateReturnsNonNull)
{
    void *ptr = pool_Allocate();
    TEST_ASSERT_NOT_NULL(ptr);
}

/* ================================================================== */
/*  Test runners                                                       */
/* ================================================================== */

TEST_GROUP_RUNNER(Pool)
{
    /* Allocation */
    RUN_TEST_CASE(Pool, AllocateReturnsNonNull);
}