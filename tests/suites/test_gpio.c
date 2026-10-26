#include "ttest.h"
#include "driver_gpio.h"

/* GPIOB reset values (RM0383, 8.4) */
#define GPIOB_MODER_RESET     0x00000280U
#define GPIOB_OSPEEDR_RESET   0x000000C0U
#define GPIOB_PUPDR_RESET     0x00000100U

//static const gpio_pin_t PB5 = { GPIOB, GPIO_PIN_5 };

static void setup(void)
{
    RCC->AHB1ENR  |=  RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1RSTR |=  RCC_AHB1RSTR_GPIOBRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST;
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
}