#include "ttest.h"
#include "driver_systick.h"

#define CORE_CLOCK_HZ   16000000U                   /* HSI */
#define CYCLES_PER_MS   (CORE_CLOCK_HZ / 1000U)

/* Helpers */


/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

static void setup(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    //cycle_counter_start();
}

/* ==================================================================== */
/*  Init                                                                */
/* ==================================================================== */

static void test_init_load_1ms(void)
{
    setup();

    systick_init(CORE_CLOCK_HZ);

    TEST_ASSERT_EQ(CYCLES_PER_MS - 1U, SysTick->LOAD);
}


/* ==================================================================== */
/*  Time                                                                */
/* ==================================================================== */

/* ==================================================================== */
/*  Timeout                                                             */
/* ==================================================================== */

/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

 void test_systick_suite(void)
  {
      /* Init */
      RUN_TEST(test_init_load_1ms);
      //RUN_TEST(test_init_load_other_clock);
      //RUN_TEST(test_init_ctrl);

      /* Time */
      //RUN_TEST(test_get_ms_counts);
      //RUN_TEST(test_init_restarts_counter);
      //RUN_TEST(test_deinit_stops_counter);

      /* Timeout */
      //RUN_TEST(test_timeout_expires);
      //RUN_TEST(test_timeout_not_expired_yet);
      //RUN_TEST(test_timeout_started_late);
      //RUN_TEST(test_timeout_zero_expires_now);

      systick_deinit();
  }
