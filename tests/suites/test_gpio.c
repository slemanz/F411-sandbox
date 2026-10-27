#include "ttest.h"
#include "driver_gpio.h"

/* GPIOB reset values (RM0383, 8.4) */
#define GPIOB_MODER_RESET     0x00000280U
#define GPIOB_OSPEEDR_RESET   0x000000C0U
#define GPIOB_PUPDR_RESET     0x00000100U
#define GPIOB_OTYPER_RESET    0x00000000U

/* GPIOA reset value (PA13/PA14/PA15 in AF for SWD/JTAG) */
#define GPIOA_MODER_RESET     0xA8000000U

//static const gpio_pin_t PB5 = { GPIOB, GPIO_PIN_5 };

static void setup(void)
{
    RCC->AHB1ENR  |=  RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1RSTR |=  RCC_AHB1RSTR_GPIOARST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOARST;

    RCC->AHB1ENR  |=  RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1RSTR |=  RCC_AHB1RSTR_GPIOBRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST;

    RCC->AHB1ENR  |=  RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1RSTR |=  RCC_AHB1RSTR_GPIOCRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOCRST;
}

static void test_setup_leaves_reset_values(void)
{
    setup();
    
    TEST_ASSERT_EQ(GPIOB_MODER_RESET, GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET, GPIOB->OSPEEDR);
    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET,   GPIOB->PUPDR);
}

/* ==================================================================== */
/*  Clock                                                               */
/* ==================================================================== */

static void test_clock_enable_gpiob(void)
{
    RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN;

    gpio_clock_enable(GPIOB);

    TEST_ASSERT_EQ(RCC_AHB1ENR_GPIOBEN, RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN);
}

static void test_clock_enable_gpioc(void)
{
    RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;

    gpio_clock_enable(GPIOC);

    TEST_ASSERT_EQ(RCC_AHB1ENR_GPIOCEN, RCC->AHB1ENR & RCC_AHB1ENR_GPIOCEN);
}

static void test_clock_disable_gpioa(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    gpio_clock_disable(GPIOA);

    TEST_ASSERT_EQ(0x0, RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN);
}

static void test_clock_disable_gpiod(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    gpio_clock_disable(GPIOD);

    TEST_ASSERT_EQ(0x0, RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN);
}

/* ==================================================================== */
/*  Pin Helper                                                          */
/* ==================================================================== */

static void test_pin_helper_pb13(void)
{
    gpio_pin_t pin = gpio_pin(GPIOB, GPIO_PIN_13);

    TEST_ASSERT_EQ((uint32_t)GPIOB, (uint32_t)pin.port);
    TEST_ASSERT_EQ(GPIO_PIN_13, pin.number);
}

static void test_pin_helper_pa5(void)
{
    gpio_pin_t pin = gpio_pin(GPIOA, GPIO_PIN_5);

    TEST_ASSERT_EQ((uint32_t)GPIOA, (uint32_t)pin.port);
    TEST_ASSERT_EQ(GPIO_PIN_5, pin.number);
}

/* ==================================================================== */
/*  Configuration                                                       */
/* ==================================================================== */

static void test_set_mode_output(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_mode(pb5, GPIO_MODE_OUTPUT);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x1U << 10), GPIOB->MODER);
}

static void test_set_mode_input(void)   /* input = 00: pin must start != 00 to be RED */
{
    setup();
    GPIOB->MODER |= (0x3U << 10);       /* arrange via register, not the driver */

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_mode(pb5, GPIO_MODE_INPUT);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET, GPIOB->MODER);
}

static void test_set_mode_analog_gpioa(void)    /* breaks a port hardcoded to GPIOB */
{
    setup();

    gpio_pin_t pa0 = gpio_pin(GPIOA, GPIO_PIN_0);
    gpio_set_mode(pa0, GPIO_MODE_ANALOG);

    TEST_ASSERT_EQ(GPIOA_MODER_RESET | (0x3U << 0), GPIOA->MODER);
}

static void test_set_mode_af(void)
{
    setup();

    gpio_pin_t pb6 = gpio_pin(GPIOB, GPIO_PIN_6);
    gpio_set_mode(pb6, GPIO_MODE_ALTFN);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x2U << 12), GPIOB->MODER);
}

static void test_set_mode_high_pin(void)    /* breaks a fixed-shift fake */
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set_mode(pb15, GPIO_MODE_ANALOG);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x3U << 30), GPIOB->MODER);
}

static void test_set_mode_overwrites(void)  /* breaks "|=" without clearing first */
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_mode(pb5, GPIO_MODE_ANALOG);
    gpio_set_mode(pb5, GPIO_MODE_INPUT);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET, GPIOB->MODER);
}

static void test_set_otype_od(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_otype(pb5, GPIO_OTYPE_OD);

    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET | (1U << 5), GPIOB->OTYPER);
}

static void test_set_otype_pp(void)
{
    setup();
    GPIOB->OTYPER |= (1U << 5);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_otype(pb5, GPIO_OTYPE_PP);

    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET, GPIOB->OTYPER);
}

static void test_set_otype_high_pin(void)
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set_otype(pb15, GPIO_OTYPE_OD);

    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET | (1U << 15), GPIOB->OTYPER);
}

static void test_set_otype_keeps_neighbors(void)
{
    setup();
    GPIOB->OTYPER |= (1U << 6);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_otype(pb5, GPIO_OTYPE_OD);

    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET | (1U << 6) | (1U << 5), GPIOB->OTYPER);
}


/* ==================================================================== */
/*  Test Suite                                                          */
/* ==================================================================== */

void test_gpio_suite(void)
{
    RUN_TEST(test_setup_leaves_reset_values);

    /* Clock */
    RUN_TEST(test_clock_enable_gpiob);
    RUN_TEST(test_clock_enable_gpioc);
    RUN_TEST(test_clock_disable_gpioa);
    RUN_TEST(test_clock_disable_gpiod);
    RUN_TEST(test_pin_helper_pb13);

    /* Pin Helper */
    RUN_TEST(test_pin_helper_pa5);

    /* Configuration */
    RUN_TEST(test_set_mode_output);
    RUN_TEST(test_set_mode_input);
    RUN_TEST(test_set_mode_analog_gpioa);
    RUN_TEST(test_set_mode_af);
    RUN_TEST(test_set_mode_high_pin);
    RUN_TEST(test_set_mode_overwrites);

    RUN_TEST(test_set_otype_od);
    RUN_TEST(test_set_otype_pp);
    RUN_TEST(test_set_otype_high_pin);
    RUN_TEST(test_set_otype_keeps_neighbors);
}