#ifndef INC_DRIVER_GPIO_H_
#define INC_DRIVER_GPIO_H_

#include <stdint.h>
#include "stm32f4xx.h"

/**
 * @brief GPIO pin number inside a port
 */
typedef enum
{
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15
}gpio_pin_number_e;

/**
 * @brief GPIO pin identification (port + pin number)
 */
typedef struct
{
    GPIO_TypeDef       *port;
    gpio_pin_number_e   number;
}gpio_pin_t;

/**
 * @brief GPIO pin logic state
 */
typedef enum
{
    GPIO_STATE_RESET = 0,   /**< Logic low (0)  */
    GPIO_STATE_SET          /**< Logic high (1) */
}gpio_state_e;


/**
 * @brief GPIO pin mode
 */
typedef enum
{
    GPIO_MODE_INPUT = 0,    /**< Digital input                         */
    GPIO_MODE_OUTPUT,       /**< General purpose output                */
    GPIO_MODE_ALTFN,        /**< Alternate function (UART, SPI, TIM..) */
    GPIO_MODE_ANALOG        /**< Analog (ADC/DAC), lowest consumption  */
}gpio_mode_e;

/**
 * @brief GPIO output type
 */
typedef enum
{
    GPIO_OTYPE_PP = 0,      /**< Push-pull: drives high and low         */
    GPIO_OTYPE_OD           /**< Open-drain: drives low only (I2C, etc) */
}gpio_otype_e;

/**
 * @brief GPIO output speed
 */
typedef enum
{
    GPIO_SPEED_LOW = 0,
    GPIO_SPEED_MEDIUM = 1,
    GPIO_SPEED_FAST = 2,
    GPIO_SPEED_HIGH = 3
}gpio_speed_e;

/**
 * @brief GPIO internal pull resistor
 */
typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
}gpio_pull_e;

/**
 * @brief GPIO alternate function
 */
typedef enum
{ 
    GPIO_AF0 = 0,
    GPIO_AF1,
    GPIO_AF2,
    GPIO_AF3,
    GPIO_AF4,
    GPIO_AF5,
    GPIO_AF6,
    GPIO_AF7,
    GPIO_AF8,
    GPIO_AF9,
    GPIO_AF10,
    GPIO_AF11,
    GPIO_AF12,
    GPIO_AF13,
    GPIO_AF14,
    GPIO_AF15
}gpio_af_e;

/**
 * @brief GPIO pin configuration.
 */
typedef struct
{
    gpio_pin_t   pin;           /**< Pin to configure                     */
    gpio_mode_e  mode;          /**< Pin mode                             */
    gpio_otype_e otype;         /**< Output type                          */
    gpio_speed_e speed;         /**< Output speed                         */
    gpio_pull_e  pull;          /**< Pull resistor                        */
    gpio_af_e    af;            /**< Alternate function                   */
    gpio_state_e init_state;    /**< Output level applied before enabling */
}gpio_config_t;

/********************************************************************************************
 *                              APIs supported by this driver
 *                  for more information check the function definitions
 ********************************************************************************************/

/*
 * Pin helper (runtime)
 */

gpio_pin_t gpio_pin(GPIO_TypeDef *port, gpio_pin_number_e number);

/*
 * Clock control
 */

void gpio_clock_enable(GPIO_TypeDef *port);
void gpio_clock_disable(GPIO_TypeDef *port);

/*
 * Init and De-init
 */

void gpio_init(const gpio_config_t *config);
void gpio_init_table(const gpio_config_t *table, uint32_t len);
void gpio_deinit(gpio_pin_t pin);
void gpio_port_reset(GPIO_TypeDef *port);

/*
 * Configuration
 */

void gpio_set_mode(gpio_pin_t pin, gpio_mode_e mode);
void gpio_set_pull(gpio_pin_t pin, gpio_pull_e pull);
void gpio_set_otype(gpio_pin_t pin, gpio_otype_e otype);
void gpio_set_speed(gpio_pin_t pin, gpio_speed_e speed);
void gpio_set_af(gpio_pin_t pin, gpio_af_e af);

/*
 * Data read and write
 */

void gpio_set(gpio_pin_t pin);
void gpio_clear(gpio_pin_t pin);
void gpio_write(gpio_pin_t pin, gpio_state_e state);
gpio_state_e gpio_read(gpio_pin_t pin);
void gpio_toggle(gpio_pin_t pin);

#endif /* INC_DRIVER_GPIO_H_ */