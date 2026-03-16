/**
 * @file interface_io.c
 * @brief Data-driven IO implementation
 *
 * Each pin is described by a config entry in s_pin_configs[].
 * Generic functions operate on the config — no per-pin code generation.
 *
 * To add a new pin:
 * 1. Add an entry to s_pin_configs[]
 * No public header changes needed.
 */

#include "interface/interface.h"
#include "driver_gpio.h"

/* ------------------------------------------------------------------ */
/* Pin configuration table                                            */
/* ------------------------------------------------------------------ */

typedef struct
{
    GPIO_RegDef_t  *port;
    uint8_t         pin;
    uint8_t         default_mode;
    uint8_t         default_speed;
    uint8_t         default_otype;
    uint8_t         default_pupd;
} io_pin_config_t;

static const io_pin_config_t s_pin_configs[] = {
/* [pin_id] = { port,  pin,             mode,           speed,          otype,        pupd           } */
    [0] = { GPIOC, GPIO_PIN_NO_13, GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PC13 */
    [1] = { GPIOA, GPIO_PIN_NO_5,  GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PA5  */
    [2] = { GPIOB, GPIO_PIN_NO_3,  GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PB3  */
    [3] = { GPIOB, GPIO_PIN_NO_4,  GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PB4  */
    [4] = { GPIOB, GPIO_PIN_NO_5,  GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PB5  */
    [5] = { GPIOA, GPIO_PIN_NO_0,  GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_OP_TYPE_PP, GPIO_NO_PUPD },  /* PA0  */
};

#define IO_PIN_COUNT  ((uint8_t)(sizeof(s_pin_configs) / sizeof(s_pin_configs[0])))

/* ------------------------------------------------------------------ */
/* Initialization tracking (bitmask)                                  */
/* ------------------------------------------------------------------ */

static uint32_t s_init_flags = 0u;

static bool pin_is_init(uint8_t pin_id)
{
    return (s_init_flags & (1u << pin_id)) != 0u;
}

static void pin_set_init(uint8_t pin_id)
{
    s_init_flags |= (1u << pin_id);
}

static void pin_clear_init(uint8_t pin_id)
{
    s_init_flags &= ~(1u << pin_id);
}

/* ------------------------------------------------------------------ */
/* Validation                                                         */
/* ------------------------------------------------------------------ */

static const io_pin_config_t *io_get_config(uint8_t pin_id)
{
    if (pin_id >= IO_PIN_COUNT) return NULL;
    return &s_pin_configs[pin_id];
}

/* ------------------------------------------------------------------ */
/* Internal: lazy-init helper                                         */
/* ------------------------------------------------------------------ */

static io_status_t io_ensure_init(uint8_t pin_id, const io_pin_config_t *cfg)
{
    if (pin_is_init(pin_id)) return IO_OK;

    GPIO_PinConfig_t gpio_cfg;
    gpio_cfg.pGPIOx              = cfg->port;
    gpio_cfg.GPIO_PinNumber      = cfg->pin;
    gpio_cfg.GPIO_PinMode        = cfg->default_mode;
    gpio_cfg.GPIO_PinSpeed       = cfg->default_speed;
    gpio_cfg.GPIO_PinOPType      = cfg->default_otype;
    gpio_cfg.GPIO_PinPuPdControl = cfg->default_pupd;
    gpio_cfg.GPIO_PinAltFunMode  = GPIO_PIN_NO_ALTFN;

    if (GPIO_Init(&gpio_cfg) != GPIO_OK) return IO_ERR_HW_FAULT;

    pin_set_init(pin_id);
    return IO_OK;
}

/* ================================================================== */
/* Public functions declared in interface.h                           */
/* ================================================================== */

io_status_t IO_init(uint8_t pin_id)
{
    const io_pin_config_t *cfg = io_get_config(pin_id);
    if (cfg == NULL) return IO_ERR_INVALID_PIN;
    return io_ensure_init(pin_id, cfg);
}

io_status_t IO_write(uint8_t pin_id, uint8_t value)
{
    const io_pin_config_t *cfg = io_get_config(pin_id);
    if (cfg == NULL) return IO_ERR_INVALID_PIN;

    io_status_t s = io_ensure_init(pin_id, cfg);
    if (s != IO_OK) return s;

    GPIO_WriteToOutputPin(cfg->port, cfg->pin, value);
    return IO_OK;
}

io_status_t IO_read(uint8_t pin_id, uint8_t *out_value)
{
    if (out_value == NULL) return IO_ERR_NULL;

    const io_pin_config_t *cfg = io_get_config(pin_id);
    if (cfg == NULL) return IO_ERR_INVALID_PIN;

    io_status_t s = io_ensure_init(pin_id, cfg);
    if (s != IO_OK) return s;

    *out_value = GPIO_ReadFromInputPin(cfg->port, cfg->pin);
    return IO_OK;
}

io_status_t IO_toggle(uint8_t pin_id)
{
    const io_pin_config_t *cfg = io_get_config(pin_id);
    if (cfg == NULL) return IO_ERR_INVALID_PIN;

    io_status_t s = io_ensure_init(pin_id, cfg);
    if (s != IO_OK) return s;

    GPIO_ToggleOutputPin(cfg->port, cfg->pin);
    return IO_OK;
}

io_status_t IO_configure(uint8_t pin_id, uint8_t option, uint8_t value)
{
    const io_pin_config_t *cfg = io_get_config(pin_id);
    if (cfg == NULL) return IO_ERR_INVALID_PIN;

    io_status_t s = io_ensure_init(pin_id, cfg);
    if (s != IO_OK) return s;

    switch (option)
    {
        case IO_OPT_MODE:        GPIO_SetPinMode      (cfg->port, cfg->pin, value); break;
        case IO_OPT_PULL:        GPIO_SetPinPull      (cfg->port, cfg->pin, value); break;
        case IO_OPT_SPEED:       GPIO_SetPinSpeed     (cfg->port, cfg->pin, value); break;
        case IO_OPT_OUTPUT_TYPE: GPIO_SetPinOutputType(cfg->port, cfg->pin, value); break;
        default: return IO_ERR_INVALID_PIN;
    }
    return IO_OK;
}

io_status_t IO_deinit(uint8_t pin_id)
{
    if (pin_id >= IO_PIN_COUNT) return IO_ERR_INVALID_PIN;
    pin_clear_init(pin_id);
    return IO_OK;
}