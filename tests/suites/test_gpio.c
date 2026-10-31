#include "ttest.h"
#include "driver_gpio.h"

/* GPIOA reset value (PA13/PA14/PA15 in AF for SWD/JTAG) */
#define GPIOA_MODER_RESET     0xA8000000U

/* GPIOB reset values (RM0383, 8.4) */
#define GPIOB_MODER_RESET     0x00000280U
#define GPIOB_OSPEEDR_RESET   0x000000C0U
#define GPIOB_PUPDR_RESET     0x00000100U
#define GPIOB_OTYPER_RESET    0x00000000U
#define GPIOB_AFRL_RESET      0x00000000U
#define GPIOB_AFRH_RESET      0x00000000U
#define GPIOB_ODR_RESET       0x00000000U

#define GPIOC_ODR_RESET       0x00000000U

/* Helpers */
static void pull_delay(void) 
{
    for(volatile uint32_t i = 0; i < 100; i++);
}

/* ==================================================================== */
/*  Setup                                                               */
/* ==================================================================== */

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

static void test_set_speed_low(void)    /* low = 00: pin must start != 00 to be RED */
{
    setup();
    GPIOB->OSPEEDR |= (0x3U << 10);     /* arrange via register, not the driver */

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_speed(pb5, GPIO_SPEED_LOW);

    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET, GPIOB->OSPEEDR);
}

static void test_set_speed_medium(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_speed(pb5, GPIO_SPEED_MEDIUM);

    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET | (0x1U << 10), GPIOB->OSPEEDR);
}

static void test_set_speed_high(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_speed(pb5, GPIO_SPEED_HIGH);

    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET | (0x3U << 10), GPIOB->OSPEEDR);
}

static void test_set_speed_high_pin(void)   /* breaks a fixed-shift fake */
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set_speed(pb15, GPIO_SPEED_FAST);

    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET | (0x2U << 30), GPIOB->OSPEEDR);
}

static void test_set_pull_none(void)
{
    setup();
    GPIOB->PUPDR |= (0x2U << 10);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_pull(pb5, GPIO_PULL_NONE);

    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET, GPIOB->PUPDR);
}

static void test_set_pull_up(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);  
    gpio_set_pull(pb5, GPIO_PULL_UP);

    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET | (0x1U << 10), GPIOB->PUPDR);
}

static void test_set_pull_down(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_pull(pb5, GPIO_PULL_DOWN);

    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET | (0x2U << 10), GPIOB->PUPDR);
}

static void test_set_pull_high_pin(void)
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set_pull(pb15, GPIO_PULL_DOWN);

    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET | (0x2U << 30), GPIOB->PUPDR);
}

static void test_set_af_low_reg(void)
{
    setup();

    gpio_pin_t pb6 = gpio_pin(GPIOB, GPIO_PIN_6);
    gpio_set_af(pb6, GPIO_AF7);

    TEST_ASSERT_EQ(GPIOB_AFRL_RESET | (0x7U << 24), GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_AFRH_RESET, GPIOB->AFR[1]);
}

static void test_set_af_high_reg(void)
{
    setup();

    gpio_pin_t pb10 = gpio_pin(GPIOB, GPIO_PIN_10);
    gpio_set_af(pb10, GPIO_AF7);

    TEST_ASSERT_EQ(GPIOB_AFRL_RESET, GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_AFRH_RESET | (0x7U << 8), GPIOB->AFR[1]);
}

static void test_set_af_high_pin(void)
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set_af(pb15, GPIO_AF15);

    TEST_ASSERT_EQ(GPIOB_AFRL_RESET, GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_AFRH_RESET | (0xFU << 28), GPIOB->AFR[1]);
}

static void test_set_af0(void)
{
    setup();
    GPIOB->AFR[0] |= (0xFU << 20);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set_af(pb5, GPIO_AF0);

    TEST_ASSERT_EQ(GPIOB_AFRL_RESET, GPIOB->AFR[0]);
}

static void test_set_af_keeps_neighbors(void)
{
    setup();
    GPIOB->AFR[0] |= (0x7U << 28);

    gpio_pin_t pb6 = gpio_pin(GPIOB, GPIO_PIN_6);
    gpio_set_af(pb6, GPIO_AF7);

    TEST_ASSERT_EQ(GPIOB_AFRL_RESET | (0x7U << 28) | (0x7U << 24), GPIOB->AFR[0]);
}

/* ==================================================================== */
/*  Init                                                                */
/* ==================================================================== */

static void test_init_output(void)
{
    setup();  

    gpio_config_t pb5 = {
        .pin        = {GPIOB, GPIO_PIN_5},
        .mode       = GPIO_MODE_OUTPUT,
        .otype      = GPIO_OTYPE_OD,
        .speed      = GPIO_SPEED_HIGH,
        .pull       = GPIO_PULL_UP,
        .init_state = GPIO_STATE_SET
    };
    gpio_init(&pb5);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET   | (0x1U << 10), GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET  | (0x1U << 5),  GPIOB->OTYPER);
    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET | (0x3U << 10), GPIOB->OSPEEDR);
    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET   | (0x1U << 10), GPIOB->PUPDR);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET, GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET     | (0x1U << 5),  GPIOB->ODR);
}

static void test_init_af(void)
{
    setup();

    gpio_config_t pb6 = {
        .pin  = { GPIOB, GPIO_PIN_6 },
        .mode = GPIO_MODE_ALTFN,
        .af   = GPIO_AF7
    };
    gpio_init(&pb6);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x2U << 12), GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET  | (0x7U << 24), GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_AFRH_RESET, GPIOB->AFR[1]);
}

static void test_init_enables_clock(void)
{
    setup();
    RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN;

    gpio_config_t pb5 = {
        .pin  = { GPIOB, GPIO_PIN_5 },
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_init(&pb5);

    TEST_ASSERT_EQ(RCC_AHB1ENR_GPIOBEN, RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN);
    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x1U << 10), GPIOB->MODER);
}

static void test_init_state_reset(void)
{
    setup();
    GPIOB->ODR |= (1U << 5);

    gpio_config_t pb5 = {
        .pin        = { GPIOB, GPIO_PIN_5 },
        .mode       = GPIO_MODE_OUTPUT,
        .init_state = GPIO_STATE_RESET
    };
    gpio_init(&pb5);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET, GPIOB->ODR);
}

  static void test_init_applies_zero_fields(void)
  {
    setup();
    GPIOB->OTYPER  |= (0x1U << 5);
    GPIOB->OSPEEDR |= (0x3U << 10);
    GPIOB->PUPDR   |= (0x1U << 10);
    GPIOB->AFR[0]  |= (0xFU << 20);

    gpio_config_t pb5 = {
        .pin  = { GPIOB, GPIO_PIN_5 },
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_init(&pb5);

    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET,  GPIOB->OTYPER);
    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET, GPIOB->OSPEEDR);
    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET,   GPIOB->PUPDR);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET,    GPIOB->AFR[0]);
}

/* ==================================================================== */
/*  Data read and write                                                 */
/* ==================================================================== */

static void test_set_pin(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set(pb5);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 5), GPIOB->ODR);
}

static void test_set_keeps_neighbors(void)
{
    setup();
    GPIOB->ODR |= (1U << 6);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_set(pb5);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 6) | (1U << 5), GPIOB->ODR);
}

static void test_set_high_pin(void)
{
    setup();

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_set(pb15);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 15), GPIOB->ODR);
}

static void test_clear_pin(void)
{
    setup();
    GPIOB->ODR |= (1U << 6) | (1U << 5);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_clear(pb5);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 6), GPIOB->ODR);
}

static void test_clear_high_pin(void)
{
    setup();
    GPIOB->ODR |= (1U << 15) | (1U << 14);

    gpio_pin_t pb15 = gpio_pin(GPIOB, GPIO_PIN_15);
    gpio_clear(pb15);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 14), GPIOB->ODR);
}

static void test_write_set(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_write(pb5, GPIO_STATE_SET);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 5), GPIOB->ODR);
}

static void test_write_reset(void)
{
    setup();
    GPIOB->ODR |= (1U << 5);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_write(pb5, GPIO_STATE_RESET);

    TEST_ASSERT_EQ(GPIOB_ODR_RESET, GPIOB->ODR);
}

static void test_read_pull_up(void)
{
    setup();
    GPIOA->PUPDR |= (0x1U << 8);
    pull_delay();

    gpio_pin_t pa4 = gpio_pin(GPIOA, GPIO_PIN_4);

    TEST_ASSERT_EQ(GPIO_STATE_SET, gpio_read(pa4));
}

static void test_read_pull_down(void)
{
    setup();
    GPIOA->PUPDR |= (0x2U << 8);
    pull_delay();

    gpio_pin_t pa4 = gpio_pin(GPIOA, GPIO_PIN_4);

    TEST_ASSERT_EQ(GPIO_STATE_RESET, gpio_read(pa4));
}

static void test_toggle(void)
{
    setup();

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);

    gpio_toggle(pb5);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (1U << 5), GPIOB->ODR);

    gpio_toggle(pb5);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET, GPIOB->ODR);
}

/* ==================================================================== */
/*  Init table and De-init                                              */
/* ==================================================================== */

static void test_init_table(void)
{
    setup();

    static const gpio_config_t table[] = {
        { .pin = { GPIOB, GPIO_PIN_5 }, .mode = GPIO_MODE_OUTPUT },
        { .pin = { GPIOB, GPIO_PIN_6 }, .mode = GPIO_MODE_ALTFN, .af = GPIO_AF7 },
    };
    gpio_init_table(table, 2);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x2U << 12) | (0x1U << 10), GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET  | (0x7U << 24), GPIOB->AFR[0]);
}

static void test_init_table_respects_len(void) /* breaks a fixed loop count */
{
    setup();

    static const gpio_config_t table[] = {
        { .pin = { GPIOB, GPIO_PIN_5 }, .mode = GPIO_MODE_OUTPUT },
        { .pin = { GPIOB, GPIO_PIN_6 }, .mode = GPIO_MODE_OUTPUT },
    };
    gpio_init_table(table, 1);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET | (0x1U << 10), GPIOB->MODER);
}

static void test_deinit(void)           /* PB6 configured too: must stay untouched */
{
    setup();
    GPIOB->MODER   |= (0x1U << 12) | (0x1U << 10);    /* arrange PB5 and PB6: output, OD, */
    GPIOB->OTYPER  |= (0x1U << 6)  | (0x1U << 5);     /* high speed, pull-up, AF15, high  */
    GPIOB->OSPEEDR |= (0x3U << 12) | (0x3U << 10);
    GPIOB->PUPDR   |= (0x1U << 12) | (0x1U << 10);
    GPIOB->AFR[0]  |= (0xFU << 24) | (0xFU << 20);
    GPIOB->ODR     |= (0x1U << 6)  | (0x1U << 5);

    gpio_pin_t pb5 = gpio_pin(GPIOB, GPIO_PIN_5);
    gpio_deinit(pb5);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET   | (0x1U << 12), GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET  | (0x1U << 6),  GPIOB->OTYPER);
    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET | (0x3U << 12), GPIOB->OSPEEDR);
    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET   | (0x1U << 12), GPIOB->PUPDR);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET    | (0xFU << 24), GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET     | (0x1U << 6),  GPIOB->ODR);
}

static void test_deinit_high_pin(void)  /* AFR[1] side */
{
    setup();
    GPIOB->MODER  |= (0x2U << 20);
    GPIOB->AFR[1] |= (0x7U << 8);

    gpio_pin_t pb10 = gpio_pin(GPIOB, GPIO_PIN_10);
    gpio_deinit(pb10);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET, GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_AFRH_RESET,  GPIOB->AFR[1]);
}

/* ==================================================================== */
/*  Port reset                                                          */
/* ==================================================================== */

static void test_port_reset(void)
{
    setup();
    GPIOB->MODER   |= (0x1U << 10);
    GPIOB->OTYPER  |= (0x1U << 5);
    GPIOB->OSPEEDR |= (0x3U << 10);
    GPIOB->PUPDR   |= (0x1U << 10);
    GPIOB->AFR[0]  |= (0xFU << 20);
    GPIOB->ODR     |= (0x1U << 5);

    gpio_port_reset(GPIOB);

    TEST_ASSERT_EQ(GPIOB_MODER_RESET,   GPIOB->MODER);
    TEST_ASSERT_EQ(GPIOB_OTYPER_RESET,  GPIOB->OTYPER);
    TEST_ASSERT_EQ(GPIOB_OSPEEDR_RESET, GPIOB->OSPEEDR);
    TEST_ASSERT_EQ(GPIOB_PUPDR_RESET,   GPIOB->PUPDR);
    TEST_ASSERT_EQ(GPIOB_AFRL_RESET,    GPIOB->AFR[0]);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET,     GPIOB->ODR);
}

static void test_port_reset_releases(void)
{
    setup();

    gpio_port_reset(GPIOB);

    TEST_ASSERT_EQ(0x0, RCC->AHB1RSTR);
}

static void test_port_reset_only_that_port(void)
{
    setup();
    GPIOB->ODR |= (0x1U << 5);
    GPIOC->ODR |= (0x1U << 13);

    gpio_port_reset(GPIOC);

    TEST_ASSERT_EQ(GPIOC_ODR_RESET, GPIOC->ODR);
    TEST_ASSERT_EQ(GPIOB_ODR_RESET | (0x1U << 5), GPIOB->ODR);
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

    RUN_TEST(test_set_speed_low);
    RUN_TEST(test_set_speed_medium);
    RUN_TEST(test_set_speed_high);
    RUN_TEST(test_set_speed_high_pin);

    RUN_TEST(test_set_pull_none);
    RUN_TEST(test_set_pull_up);
    RUN_TEST(test_set_pull_down);
    RUN_TEST(test_set_pull_high_pin);

    RUN_TEST(test_set_af_low_reg);
    RUN_TEST(test_set_af_high_reg);
    RUN_TEST(test_set_af_high_pin);
    RUN_TEST(test_set_af0);
    RUN_TEST(test_set_af_keeps_neighbors);

    /* Init */
    RUN_TEST(test_init_output);
    RUN_TEST(test_init_af);
    RUN_TEST(test_init_enables_clock);
    RUN_TEST(test_init_state_reset);
    RUN_TEST(test_init_applies_zero_fields);

    /* Data read and write */
    RUN_TEST(test_set_pin);
    RUN_TEST(test_set_keeps_neighbors);
    RUN_TEST(test_set_high_pin);
    RUN_TEST(test_clear_pin);
    RUN_TEST(test_clear_high_pin);
    RUN_TEST(test_write_set);
    RUN_TEST(test_write_reset);

    RUN_TEST(test_read_pull_up);
    RUN_TEST(test_read_pull_down);
    RUN_TEST(test_toggle);

    /* Init table and De-init */
    RUN_TEST(test_init_table);
    RUN_TEST(test_init_table_respects_len);
    RUN_TEST(test_deinit);
    RUN_TEST(test_deinit_high_pin);

    /* Port reset */
    RUN_TEST(test_port_reset);
    RUN_TEST(test_port_reset_releases);
    RUN_TEST(test_port_reset_only_that_port);
}