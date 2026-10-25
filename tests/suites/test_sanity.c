#include "ttest.h"

static void test_pass(void)
{
    TEST_ASSERT_EQ(4, 2 + 2);
}

static void test_fail(void)
{
    while(1)
    {

    }
    TEST_ASSERT_EQ(5, 2 + 2);
}

void test_sanity_suite(void)
{
    RUN_TEST(test_pass);
    RUN_TEST(test_fail);
}