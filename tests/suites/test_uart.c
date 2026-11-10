#include "ttest.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "driver_systick.h"

#define PCLK_HZ             16000000U
#define UART_RX_TRIES       100000U

/* ==================================================================== */
/*  Helpers                                                             */
/* ==================================================================== */

static bool uart_read_byte_wait(uint8_t *byte)
{
    for(uint32_t i = 0; i < UART_RX_TRIES; i++)
    {
        if(uart_read_byte(USART1, byte)) return true;
    }

    return false;
}

/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

/* Loopback: jumper PA9 (TX) -> PA10 (RX) */
static const gpio_config_t uart_pins[] = {
    { .pin = { GPIOA, GPIO_PIN_9  }, .mode = GPIO_MODE_ALTFN, .af = GPIO_AF7 },
    { .pin = { GPIOA, GPIO_PIN_10 }, .mode = GPIO_MODE_ALTFN, .af = GPIO_AF7, .pull = GPIO_PULL_UP },       
};

static const uart_config_t uart = {
    .instance = USART1,
    .clock_hz = PCLK_HZ,
    .baudrate = 115200,
};

static void setup(void)
{
    RCC->APB2ENR  |=  (RCC_APB2ENR_USART1EN | RCC_APB2ENR_USART6EN);
    RCC->APB2RSTR |=  (RCC_APB2RSTR_USART1RST | RCC_APB2RSTR_USART6RST);
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART1RST | RCC_APB2RSTR_USART6RST);
}

static void uart_loopback_setup(void)
{
    setup();
    gpio_init_table(uart_pins, 2);
    uart_init(&uart);
}

static void uart_no_tx_setup(void)
{
    setup();
    systick_init(PCLK_HZ);

    uart_config_t config = uart;
    config.mode = UART_MODE_RX;
    uart_init(&config);
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

static void test_set_disabled_keeps_others(void)
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

static void test_mode_overwrites(void)
{
    setup();

    uart_set_mode(USART1, UART_MODE_TX_RX);
    uart_set_mode(USART1, UART_MODE_TX);

    TEST_ASSERT_EQ(USART_CR1_TE, USART1->CR1);
}

static void test_mode_usart6(void)
{
    setup();

    uart_set_mode(USART6, UART_MODE_RX);

    TEST_ASSERT_EQ(0U, USART1->CR1);
    TEST_ASSERT_EQ(USART_CR1_RE, USART6->CR1);
}

/* ==================================================================== */
/* Parity                                                               */
/* ==================================================================== */

static void test_parity_even(void)
{
    setup();

    uart_set_parity(USART1, UART_PARITY_EVEN);

    TEST_ASSERT_EQ(USART_CR1_M | USART_CR1_PCE, USART1->CR1);
}

static void test_parity_odd(void)
{
    setup();

    uart_set_parity(USART1, UART_PARITY_ODD);

    TEST_ASSERT_EQ(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS, USART1->CR1);
}

static void test_parity_odd_to_even(void)
{
    setup();

    uart_set_parity(USART1, UART_PARITY_ODD);
    uart_set_parity(USART1, UART_PARITY_EVEN);

    TEST_ASSERT_EQ(USART_CR1_M | USART_CR1_PCE, USART1->CR1);
}

static void test_parity_none_clears(void)
{
    setup();

    uart_set_parity(USART1, UART_PARITY_ODD);
    uart_set_parity(USART1, UART_PARITY_NONE);

    TEST_ASSERT_EQ(0U, USART1->CR1);
}

static void test_parity_keeps_others(void)
{
    setup();
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    uart_set_parity(USART1, UART_PARITY_EVEN);

    TEST_ASSERT_EQ(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_M  | USART_CR1_PCE, USART1->CR1);
}

/* ==================================================================== */
/*  Stop Bits                                                           */
/* ==================================================================== */

static void test_stop_bits_2(void)
{
    setup();

    uart_set_stop_bits(USART1, UART_STOP_BITS_2);

    TEST_ASSERT_EQ(USART_CR2_STOP_1, USART1->CR2);
}

static void test_stop_bits_back_to_1(void)
{
    setup();

    uart_set_stop_bits(USART1, UART_STOP_BITS_2);
    uart_set_stop_bits(USART1, UART_STOP_BITS_1);

    TEST_ASSERT_EQ(0U, USART1->CR2);
}

/* ==================================================================== */
/*  Init/Deinit                                                         */
/* ==================================================================== */

static void test_init_default(void)
{
    setup();
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    uart_config_t config = { .instance = USART1, .clock_hz = PCLK_HZ, .baudrate = 115200 };

    uart_init(&config);

    TEST_ASSERT_EQ(RCC_APB2ENR_USART1EN, RCC->APB2ENR & RCC_APB2ENR_USART1EN);
    TEST_ASSERT_EQ(139U, USART1->BRR);
    TEST_ASSERT_EQ(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE, USART1->CR1);
    TEST_ASSERT_EQ(0U, USART1->CR2);
}

static void test_init_all_fields(void)
{
    setup();
    uart_config_t config = {
        .instance = USART6, .clock_hz = PCLK_HZ, .baudrate = 9600,
        .mode = UART_MODE_TX, .parity = UART_PARITY_ODD, .stop_bits = UART_STOP_BITS_2,
    };

    uart_init(&config);

    TEST_ASSERT_EQ(1667U, USART6->BRR);
    TEST_ASSERT_EQ(USART_CR1_UE | USART_CR1_TE | USART_CR1_M  | USART_CR1_PCE | USART_CR1_PS, USART6->CR1);
    TEST_ASSERT_EQ(USART_CR2_STOP_1, USART6->CR2);
}

static void test_deinit(void)
{
    setup();
    uart_config_t config = { .instance = USART1, .clock_hz = PCLK_HZ, .baudrate = 115200 };
    uart_init(&config);

    uart_deinit(USART1);

    TEST_ASSERT_EQ(0U, RCC->APB2ENR & RCC_APB2ENR_USART1EN);

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    TEST_ASSERT_EQ(0U, USART1->CR1);
}

/* ==================================================================== */
/*  Write/Read (loopback)                                               */
/* ==================================================================== */

static void test_read_empty(void)
{
    uart_loopback_setup();
    uint8_t byte = 0x55;

    TEST_ASSERT_EQ(false, uart_read_byte(USART1, &byte));
    TEST_ASSERT_EQ(0x55U, byte);
}

static void test_loopback_byte(void)
{
    uart_loopback_setup();
    uint8_t byte = 0;

    uart_write_byte(USART1, 0xA5);

    TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
    TEST_ASSERT_EQ(0xA5U, byte);
}

static void test_loopback_write(void)
{
    uart_loopback_setup();
    const uint8_t data[] = { 0x01, 0x80, 0xFF };
    uint8_t byte = 0;

    uart_write(USART1, data, 3);

    for(uint32_t i = 0; i < 3; i++)
    {
        TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
        TEST_ASSERT_EQ(data[i], byte);
    }
}

static void test_loopback_write_string(void)
{
    uart_loopback_setup();
    uint8_t byte = 0;

    uart_write_string(USART1, "Hi!");

    TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
    TEST_ASSERT_EQ('H', byte);
    TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
    TEST_ASSERT_EQ('i', byte);
    TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
    TEST_ASSERT_EQ('!', byte);
    TEST_ASSERT_EQ(false, uart_read_byte_wait(&byte)); 
}

static void test_flush_waits_tc(void)
{
    uart_loopback_setup();

    uart_write_byte(USART1, 0xA5);
    uart_flush(USART1);

    TEST_ASSERT_EQ(USART_SR_TC, USART1->SR & USART_SR_TC);
}

static void test_read_clears_overrun(void)
{
    uart_loopback_setup();
    uint8_t byte = 0;

    uart_write_byte(USART1, 0x11);
    uart_write_byte(USART1, 0x22);
    uart_flush(USART1);

    TEST_ASSERT_EQ(USART_SR_ORE, USART1->SR & USART_SR_ORE);

    TEST_ASSERT_EQ(true, uart_read_byte(USART1, &byte));
    TEST_ASSERT_EQ(0x11U, byte);
    TEST_ASSERT_EQ(0U, USART1->SR & USART_SR_ORE);
}

/* ==================================================================== */
/*  Write with timeout                                                  */
/* ==================================================================== */

static void test_write_byte_timeout_ok(void)
{
    uart_loopback_setup();
    systick_init(PCLK_HZ);
    uint8_t byte = 0;

    TEST_ASSERT_EQ(true, uart_write_byte_timeout(USART1, 0xA5, 5));

    TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
    TEST_ASSERT_EQ(0xA5U, byte);
}

static void test_write_byte_timeout_expires(void)
{
    uart_no_tx_setup();

    TEST_ASSERT_EQ(true, uart_write_byte_timeout(USART1, 0x11, 5));   /* DR was empty */

    uint32_t start = systick_get_ms();
    TEST_ASSERT_EQ(false, uart_write_byte_timeout(USART1, 0x22, 5));  /* DR still full */
    TEST_ASSERT_TRUE(systick_get_ms() - start >= 5U);                 /* really waited */
}

static void test_write_byte_timeout_zero_ready(void)    /* flag checked before the clock */
{
    uart_no_tx_setup();

    TEST_ASSERT_EQ(true, uart_write_byte_timeout(USART1, 0x11, 0));
}

static void test_write_timeout_ok(void)
{
    uart_loopback_setup();
    systick_init(PCLK_HZ);
    const uint8_t data[] = { 0x01, 0x80, 0xFF };
    uint8_t byte = 0;

    TEST_ASSERT_EQ(true, uart_write_timeout(USART1, data, 3, 5));

    for(uint32_t i = 0; i < 3; i++)
    {
        TEST_ASSERT_EQ(true, uart_read_byte_wait(&byte));
        TEST_ASSERT_EQ(data[i], byte);
    }
}

static void test_write_timeout_expires(void)
{
    uart_no_tx_setup();
    const uint8_t data[] = { 0x01, 0x80, 0xFF };

    TEST_ASSERT_EQ(false, uart_write_timeout(USART1, data, 3, 5));
}

static void test_write_timeout_is_total(void)   /* breaks a per-byte timeout */
{
    setup();
    systick_init(PCLK_HZ);
    uart_config_t config = uart;
    config.baudrate = 9600;                     /* ~1 ms per byte */
    config.mode     = UART_MODE_TX;
    uart_init(&config);
    const uint8_t data[10] = { 0 };

    TEST_ASSERT_EQ(false, uart_write_timeout(USART1, data, 10, 5));    /* needs ~10 ms */
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

    /* Parity */
    RUN_TEST(test_parity_even);
    RUN_TEST(test_parity_odd);
    RUN_TEST(test_parity_odd_to_even);
    RUN_TEST(test_parity_none_clears);
    RUN_TEST(test_parity_keeps_others);

    /* Stop bits */
    RUN_TEST(test_stop_bits_2);
    RUN_TEST(test_stop_bits_back_to_1);

    /* Init/deinit */
    RUN_TEST(test_init_default);
    RUN_TEST(test_init_all_fields);
    RUN_TEST(test_deinit);

    /* Write/Read (loopback) */
    RUN_TEST(test_read_empty);
    RUN_TEST(test_loopback_byte);
    RUN_TEST(test_loopback_write);
    RUN_TEST(test_loopback_write_string);
    RUN_TEST(test_flush_waits_tc);
    RUN_TEST(test_read_clears_overrun);

    /* Write with timeout */
    RUN_TEST(test_write_byte_timeout_ok);
    RUN_TEST(test_write_byte_timeout_expires);
    RUN_TEST(test_write_byte_timeout_zero_ready);
    RUN_TEST(test_write_timeout_ok);
    RUN_TEST(test_write_timeout_expires);
    RUN_TEST(test_write_timeout_is_total);

    /* Flush with timeout */
    //RUN_TEST(test_flush_timeout_ok);
    //RUN_TEST(test_flush_timeout_expires);

    systick_deinit();
}