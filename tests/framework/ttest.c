#include "ttest.h"

volatile ttest_results_t ttest_results;

static const char *s_ttest_current_test;
static bool s_ttest_current_failed;

void ttest_begin(void)
{
    ttest_results.total  = 0;
    ttest_results.failed = 0; 
    ttest_results.state  = TTEST_STATE_RUNNING;
    ttest_results.magic  = TTEST_MAGIC;
}

void ttest_end(void)
{
    ttest_results.state = TTEST_STATE_DONE;

    while(1)
    {
        /* wait the host read the results */
    }
}

void ttest_run(void (*test)(void), const char *name)
{
    s_ttest_current_test = name;
    s_ttest_current_failed = false;

    test();
    ttest_results.total++;

    if(s_ttest_current_failed)
    {
        ttest_results.failed++;
    }
}

bool ttest_check(uint32_t expected, uint32_t actual, const char *file, uint32_t line)
{
    if(expected == actual)
    {
        return true;
    }

    if(ttest_results.failed == 0)
    {
        ttest_results.fail_test     = s_ttest_current_test;
        ttest_results.fail_file     = file;
        ttest_results.fail_line     = line;
        ttest_results.fail_expected = expected;
        ttest_results.fail_actual   = actual;
    }

    s_ttest_current_failed = true;
    return false;
}