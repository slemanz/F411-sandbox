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

static void test_setup_leaves_reset_values(void)
{
    setup();

    TEST_ASSERT_EQ(0U, USART1->CR1);
    TEST_ASSERT_EQ(0U, USART1->CR2);
    TEST_ASSERT_EQ(0U, USART1->BRR);
}


/* ==================================================================== */
/*  Clock                                                               */
/* ==================================================================== */

static void test_clock_enable_usart1(void)
{
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
/*  Baudrate                                                            */
/* ==================================================================== */

static void test_baudrate_115200(void)
{
    setup();

    uart_set_baudrate(USART1, PCLK_HZ, 115200);

    TEST_ASSERT_EQ(139U, USART1->BRR);
}

static void test_baudrate_9600(void)
{
    setup();

    uart_set_baudrate(USART1, PCLK_HZ, 9600);

    TEST_ASSERT_EQ(1667U, USART1->BRR);
}

/* ==================================================================== */
/*  Enable                                                              */
/* ==================================================================== */

static void test_set_enabled(void)
{
    setup();

    uart_set_enabled(USART1, true);

    TEST_ASSERT_EQ(USART_CR1_UE, USART1->CR1);
}

static void test_set_disabled_keeps_others(void)    /* breaks "CR1 = 0" */
{
    setup();
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    uart_set_enabled(USART1, false);

    TEST_ASSERT_EQ(USART_CR1_TE | USART_CR1_RE, USART1->CR1);
}

/* ==================================================================== */
/* Mode                                                                 */
/* ==================================================================== */

static void test_mode_tx_rx(void)
{
    setup();

    uart_set_mode(USART1, UART_MODE_TX_RX);

    TEST_ASSERT_EQ(USART_CR1_TE | USART_CR1_RE, USART1->CR1);
}

static void test_mode_tx(void)
{
    setup();

    uart_set_mode(USART1, UART_MODE_TX);

    TEST_ASSERT_EQ(USART_CR1_TE, USART1->CR1);
}

static void test_mode_rx(void)
{
    setup();

    uart_set_mode(USART1, UART_MODE_RX);

    TEST_ASSERT_EQ(USART_CR1_RE, USART1->CR1);
}

static void test_mode_overwrites(void)      /* breaks "|=" without clearing first */
{
    setup();

    uart_set_mode(USART1, UART_MODE_TX_RX);
    uart_set_mode(USART1, UART_MODE_TX);

    TEST_ASSERT_EQ(USART_CR1_TE, USART1->CR1);
}

static void test_mode_usart6(void)          /* breaks a fixed USART1 */
{
    setup();

    uart_set_mode(USART6, UART_MODE_RX);

    TEST_ASSERT_EQ(0U, USART1->CR1);
    TEST_ASSERT_EQ(USART_CR1_RE, USART6->CR1);
}

/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

void test_uart_suite(void)
{
    RUN_TEST(test_setup_leaves_reset_values);

    /* Clock */
    RUN_TEST(test_clock_enable_usart1);
    RUN_TEST(test_clock_enable_usart2);
    RUN_TEST(test_clock_enable_usart6);
    RUN_TEST(test_clock_disable_usart6);

    /* Baudrate */
    RUN_TEST(test_baudrate_115200);
    RUN_TEST(test_baudrate_9600);

    /* Enable */
    RUN_TEST(test_set_enabled);
    RUN_TEST(test_set_disabled_keeps_others);

    /* Mode */
    RUN_TEST(test_mode_tx_rx);
    RUN_TEST(test_mode_tx);
    RUN_TEST(test_mode_rx);
    RUN_TEST(test_mode_overwrites);
    RUN_TEST(test_mode_usart6);
}
