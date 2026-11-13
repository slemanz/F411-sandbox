#include "ttest.h"
  #include "driver_nvic.h"

/* IRQs:
 *       EXTI0 = 6 (ISER[0] bit 6), USART1 = 37 (ISER[1] bit 5),
 *       USART2 = 38 (ISER[1] bit 6), USART6 = 71 (ISER[2] bit 7)
 */

/* ==================================================================== */
/*  Helpers                                                             */
/* ==================================================================== */

static volatile uint32_t s_exti0_calls = 0;

void EXTI0_IRQHandler(void)
{
    s_exti0_calls++;
}

/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

static void setup(void)
{
    NVIC->ICER[0] = 0xFFFFFFFFU;
    NVIC->ICER[1] = 0xFFFFFFFFU;
    NVIC->ICER[2] = 0xFFFFFFFFU;

    NVIC->ICPR[0] = 0xFFFFFFFFU;
    NVIC->ICPR[1] = 0xFFFFFFFFU;
    NVIC->ICPR[2] = 0xFFFFFFFFU;

    NVIC->IPR[EXTI0_IRQn]  = 0;
    NVIC->IPR[USART1_IRQn] = 0;
    NVIC->IPR[USART6_IRQn] = 0;

    s_exti0_calls = 0;
}

static void test_setup_leaves_all_disabled(void)
{
    setup();

    TEST_ASSERT_EQ(0U, NVIC->ISER[0]);
    TEST_ASSERT_EQ(0U, NVIC->ISER[1]);
    TEST_ASSERT_EQ(0U, NVIC->ISER[2]);
}

/* ==================================================================== */
/*  Enable / Disable                                                    */
/* ==================================================================== */

static void test_enable_low_irq(void)
{
    setup();

    nvic_enable(EXTI0_IRQn);

    TEST_ASSERT_EQ(1U << 6, NVIC->ISER[0]);
}

static void test_enable_usart1(void)
{
    setup();

    nvic_enable(USART1_IRQn);

    TEST_ASSERT_EQ(0U, NVIC->ISER[0]);
    TEST_ASSERT_EQ(1U << 5, NVIC->ISER[1]);
}

static void test_enable_usart6(void)
{
    setup();

    nvic_enable(USART6_IRQn);

    TEST_ASSERT_EQ(1U << 7, NVIC->ISER[2]);
}

static void test_disable_keeps_others(void)
{
    setup();
    NVIC->ISER[1] = (1U << 5) | (1U << 6); 

    nvic_disable(USART1_IRQn);

    TEST_ASSERT_EQ(1U << 6, NVIC->ISER[1]);
}

/* ==================================================================== */
/*  Behavior                                                            */
/* ==================================================================== */

static void test_enable_runs_pending_irq(void)
{
    setup();
    NVIC->ISPR[0] = 1U << 6;

    nvic_enable(EXTI0_IRQn);
    __DSB();
    __ISB();

    TEST_ASSERT_EQ(1U, s_exti0_calls);
}

static void test_disabled_irq_stays_pending(void)
{
    setup();
    nvic_enable(EXTI0_IRQn);
    nvic_disable(EXTI0_IRQn);

    NVIC->ISPR[0] = 1U << 6;
    __DSB();
    __ISB();

    TEST_ASSERT_EQ(0U, s_exti0_calls);
    TEST_ASSERT_EQ(1U << 6, NVIC->ISPR[0] & (1U << 6));

    NVIC->ICPR[0] = 1U << 6;
}



/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

void test_nvic_suite(void)
{
    RUN_TEST(test_setup_leaves_all_disabled);

    /* Enable / Disable */
    RUN_TEST(test_enable_low_irq);
    RUN_TEST(test_enable_usart1);
    RUN_TEST(test_enable_usart6);
    RUN_TEST(test_disable_keeps_others);

    /* Behavior */
    RUN_TEST(test_enable_runs_pending_irq);
    RUN_TEST(test_disabled_irq_stays_pending);

    /* Priority */
    RUN_TEST(test_priority_usart1);
    RUN_TEST(test_priority_usart6);
}

