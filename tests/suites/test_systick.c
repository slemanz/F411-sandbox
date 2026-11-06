#include "ttest.h"
#include "driver_systick.h"

#define CORE_CLOCK_HZ   16000000U                   /* HSI */
#define CYCLES_PER_MS   (CORE_CLOCK_HZ / 1000U)

/* Helpers */
static void cycle_counter_start(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static void wait_cycles(uint32_t cycles)
{
    uint32_t start = DWT->CYCCNT;

    while((DWT->CYCCNT - start) < cycles)
    {
    }
}

/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

static void setup(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    cycle_counter_start();
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

static void test_init_load_other_clock(void)    /* breaks a fixed LOAD = 15999 */
{
    setup();

    systick_init(8000000U);

    TEST_ASSERT_EQ(7999U, SysTick->LOAD);
}

static void test_init_ctrl(void)
{
    setup();

    systick_init(CORE_CLOCK_HZ);

    TEST_ASSERT_EQ(SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk, SysTick->CTRL);
}


/* ==================================================================== */
/*  Time                                                                */
/* ==================================================================== */

static void test_get_ms_counts(void)
{
    setup();

    systick_init(CORE_CLOCK_HZ);
    wait_cycles(5U*CYCLES_PER_MS + CYCLES_PER_MS/2U);

    TEST_ASSERT_EQ(5U, systick_get_ms());
}

  static void test_init_restarts_counter(void)
  {
      setup();

      systick_init(CORE_CLOCK_HZ);
      wait_cycles(3U * CYCLES_PER_MS + CYCLES_PER_MS / 2U);
      systick_init(CORE_CLOCK_HZ);

      TEST_ASSERT_EQ(0U, systick_get_ms());
  }

  static void test_deinit_stops_counter(void)
  {
      setup();

      systick_init(CORE_CLOCK_HZ);
      wait_cycles(2U*CYCLES_PER_MS + CYCLES_PER_MS/2U);
      systick_deinit();
      wait_cycles(3U * CYCLES_PER_MS);

      TEST_ASSERT_EQ(0U, SysTick->CTRL);
      TEST_ASSERT_EQ(2U, systick_get_ms());
  }

  static void test_delay_10ms(void)
  {
      setup();
      systick_init(CORE_CLOCK_HZ);
      wait_cycles(CYCLES_PER_MS / 2U);

      uint32_t start = DWT->CYCCNT;
      systick_delay_ms(10);
      uint32_t elapsed = DWT->CYCCNT - start;

      TEST_ASSERT_TRUE(elapsed >= 10U * CYCLES_PER_MS);
      TEST_ASSERT_TRUE(elapsed <= 11U * CYCLES_PER_MS);
  }


/* ==================================================================== */
/*  Timeout                                                             */
/* ==================================================================== */

static void test_timeout_expires(void)
{
    setup();

    systick_init(CORE_CLOCK_HZ);
    systick_timeout_t timeout;
    systick_timeout_start(&timeout, 5);
    wait_cycles(5U * CYCLES_PER_MS + CYCLES_PER_MS / 2U);

    TEST_ASSERT_EQ(true, systick_timeout_expired(&timeout));
}

static void test_timeout_not_expired_yet(void)  /* breaks "return true" */
{
    setup();

    systick_init(CORE_CLOCK_HZ);
    systick_timeout_t timeout;
    systick_timeout_start(&timeout, 5);
    wait_cycles(3U * CYCLES_PER_MS + CYCLES_PER_MS / 2U);

    TEST_ASSERT_EQ(false, systick_timeout_expired(&timeout));
}

static void test_timeout_started_late(void)     /* breaks "ignore start_ms" */
{
    setup();

    systick_init(CORE_CLOCK_HZ);
    systick_timeout_t timeout;
    wait_cycles(6U * CYCLES_PER_MS + CYCLES_PER_MS / 2U);
    systick_timeout_start(&timeout, 5);
    wait_cycles(CYCLES_PER_MS);

    TEST_ASSERT_EQ(false, systick_timeout_expired(&timeout));
}

static void test_timeout_zero_expires_now(void)
{
    setup();

    systick_init(CORE_CLOCK_HZ);
    systick_timeout_t timeout;
    systick_timeout_start(&timeout, 0);

    TEST_ASSERT_EQ(true, systick_timeout_expired(&timeout));
}

/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

 void test_systick_suite(void)
  {
      /* Init */
      RUN_TEST(test_init_load_1ms);
      RUN_TEST(test_init_load_other_clock);
      RUN_TEST(test_init_ctrl);

      /* Time */
      RUN_TEST(test_get_ms_counts);
      RUN_TEST(test_init_restarts_counter);
      RUN_TEST(test_deinit_stops_counter);
      RUN_TEST(test_delay_10ms);

      /* Timeout */
      RUN_TEST(test_timeout_expires);
      RUN_TEST(test_timeout_not_expired_yet);
      RUN_TEST(test_timeout_started_late);
      RUN_TEST(test_timeout_zero_expires_now);

      systick_deinit();
  }
