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

void test_gpio_suite(void)
{
    RUN_TEST(test_setup_leaves_reset_values);
}