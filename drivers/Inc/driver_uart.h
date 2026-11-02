#ifndef INC_DRIVER_UART_H_
#define INC_DRIVER_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

/**
 * @brief UART direction
 */
typedef enum
{
    UART_MODE_TX_RX = 0,    /**< Transmit and receive */
    UART_MODE_TX,           /**< Transmit only        */
    UART_MODE_RX            /**< Receive only         */
}uart_mode_e;

/**
 * @brief UART parity
 */
typedef enum
{
    UART_PARITY_NONE = 0,   /**< No parity   */
    UART_PARITY_EVEN,       /**< Even parity */
    UART_PARITY_ODD         /**< Odd parity  */
}uart_parity_e;

/**
 * @brief UART stop bits
 */
typedef enum
{
    UART_STOP_BITS_1 = 0,   /**< 1 stop bit  */
    UART_STOP_BITS_2        /**< 2 stop bits */
}uart_stop_bits_e;

/**
 * @brief UART configuration (always 8 data bits).
 */
typedef struct
{
    USART_TypeDef    *instance;     /**< USART1, USART2 or USART6        */
    uint32_t          clock_hz;     /**< Peripheral clock (PCLK) in Hz   */
    uint32_t          baudrate;     /**< Baudrate. Ex: 115200            */
    uart_mode_e       mode;         /**< Direction                       */
    uart_parity_e     parity;       /**< Parity                          */
    uart_stop_bits_e  stop_bits;    /**< Stop bits                       */
}uart_config_t;

/********************************************************************************************
 *                              APIs supported by this driver
 *                  for more information check the function definitions
 ********************************************************************************************/

/*
 * Clock control
 */

void uart_clock_enable(USART_TypeDef *instance);
void uart_clock_disable(USART_TypeDef *instance);

/*
 * Init and De-init
 */

void uart_init(const uart_config_t *config);
void uart_deinit(USART_TypeDef *instance);

/*
 * Configuration
 */

void uart_set_enabled(USART_TypeDef *instance, bool enabled);
void uart_set_mode(USART_TypeDef *instance, uart_mode_e mode);
void uart_set_parity(USART_TypeDef *instance, uart_parity_e parity);
void uart_set_stop_bits(USART_TypeDef *instance, uart_stop_bits_e stop_bits);
void uart_set_baudrate(USART_TypeDef *instance, uint32_t clock_hz, uint32_t baudrate);

/*
 * Write (blocking)
 */

void uart_write_byte(USART_TypeDef *instance, uint8_t byte);
void uart_write(USART_TypeDef *instance, const uint8_t *data, uint32_t len);
void uart_write_string(USART_TypeDef *instance, const char *str);
void uart_flush(USART_TypeDef *instance);

/*
 * Write with timeout (needs systick_init). Return false on timeout.
 */

bool uart_write_byte_timeout(USART_TypeDef *instance, uint8_t byte, uint32_t timeout_ms);
bool uart_write_timeout(USART_TypeDef *instance, const uint8_t *data, uint32_t len, uint32_t timeout_ms);
bool uart_flush_timeout(USART_TypeDef *instance, uint32_t timeout_ms);

/*
 * Read (non-blocking)
 */

bool uart_read_byte(USART_TypeDef *instance, uint8_t *byte);

#endif /* INC_DRIVER_UART_H_ */