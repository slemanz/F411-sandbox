#include "ttest.h"
#include "driver_uart.h"

#define PCLK_HZ     16000000U

/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

static void setup(void)
{
    RCC->APB2ENR  |=  RCC_APB2ENR_USART1EN;
    RCC->APB2RSTR |=  RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
}

/* ==================================================================== */
/*  Clock                                                               */
/* ==================================================================== */

static void test_clock_enable_usart1(void)
{
    setup();

    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    uint32_t others = RCC->APB2ENR;

    uart_clock_enable(USART1);

    TEST_ASSERT_EQ(others | RCC_APB2ENR_USART1EN, RCC->APB2ENR);
}

static void test_clock_enable_usart2(void)
{
    RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
    uint32_t others = RCC->APB1ENR;

    uart_clock_enable(USART2);

    TEST_ASSERT_EQ(others | RCC_APB1ENR_USART2EN, RCC->APB1ENR);
}

static void test_clock_enable_usart6(void)
{
    RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
    uint32_t others = RCC->APB2ENR;

    uart_clock_enable(USART6);

    TEST_ASSERT_EQ(others | RCC_APB2ENR_USART6EN, RCC->APB2ENR);
}

static void test_clock_disable_usart6(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
    uint32_t others = RCC->APB2ENR & ~RCC_APB2ENR_USART6EN;

    uart_clock_disable(USART6);

    TEST_ASSERT_EQ(others, RCC->APB2ENR);
}


/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

void test_uart_suite(void)
{
    /* Clock */
    RUN_TEST(test_clock_enable_usart1);
    RUN_TEST(test_clock_enable_usart2);
    RUN_TEST(test_clock_enable_usart6);
    RUN_TEST(test_clock_disable_usart6);

    /* Baudrate */
    //RUN_TEST(test_baudrate_115200);
    //RUN_TEST(test_baudrate_9600);
}
