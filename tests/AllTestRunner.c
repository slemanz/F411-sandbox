#include "unity_fixture.h"

/* ------------------------------------------------------------------ */
/*  Forward declarations — one per TEST_GROUP_RUNNER defined           */
/* ------------------------------------------------------------------ */

void RunAllTests(void)
{
    /* shared */

    /* core */
    RUN_TEST_GROUP(SimpleTimer);

    /* bsp */
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                        */
/* ------------------------------------------------------------------ */

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}