#include "ttest.h"

void test_sanity_suite(void);

int main(void)
{
    ttest_begin();

    test_sanity_suite();

    ttest_end();
}