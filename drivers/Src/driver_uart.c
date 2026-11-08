#include "driver_uart.h"

/*
* Clock control
*/

void uart_clock_enable(USART_TypeDef *instance)
{
    if      (instance == USART1) RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    else if (instance == USART2) RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    else if (instance == USART6) RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
}

void uart_clock_disable(USART_TypeDef *instance)
{
    if      (instance == USART1) RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    else if (instance == USART2) RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
    else if (instance == USART6) RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
}

/*
* Init and De-init
*/

void uart_init(const uart_config_t *config)
{

}

void uart_deinit(USART_TypeDef *instance)
{

}

/*
* Configuration
*/

void uart_set_enabled(USART_TypeDef *instance, bool enabled)
{
    if(enabled)
    {
        instance->CR1 |= USART_CR1_UE;
    }else
    {
        instance->CR1 &= ~USART_CR1_UE;
    }
}

void uart_set_mode(USART_TypeDef *instance, uart_mode_e mode)
{
    instance->CR1 &= ~(USART_CR1_TE | USART_CR1_RE);

    if(mode != UART_MODE_RX) instance->CR1 |= USART_CR1_TE;
    if(mode != UART_MODE_TX) instance->CR1 |= USART_CR1_RE;
}

void uart_set_parity(USART_TypeDef *instance, uart_parity_e parity)
{
    instance->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS);

    if(parity != UART_PARITY_NONE)
    {
        instance->CR1 |= USART_CR1_M | USART_CR1_PCE;
    }
    if(parity == UART_PARITY_ODD)
    {
        instance->CR1 |= USART_CR1_PS;
    }
}

void uart_set_stop_bits(USART_TypeDef *instance, uart_stop_bits_e stop_bits)
{

}

void uart_set_baudrate(USART_TypeDef *instance, uint32_t clock_hz, uint32_t baudrate)
{
    instance->BRR = (clock_hz + (baudrate / 2U)) / baudrate;
}

/*
* Write (blocking)
*/

void uart_write_byte(USART_TypeDef *instance, uint8_t byte)
{

}

void uart_write(USART_TypeDef *instance, const uint8_t *data, uint32_t len)
{

}

void uart_write_string(USART_TypeDef *instance, const char *str)
{

}

void uart_flush(USART_TypeDef *instance)
{

}

/*
* Write with timeout
*/

bool uart_write_byte_timeout(USART_TypeDef *instance, uint8_t byte, uint32_t timeout_ms)
{
    return false;
}

bool uart_write_timeout(USART_TypeDef *instance, const uint8_t *data, uint32_t len, uint32_t timeout_ms)
{
    return false;
}

bool uart_flush_timeout(USART_TypeDef *instance, uint32_t timeout_ms)
{
    return false;
}

/*
* Read (non-blocking)
*/

bool uart_read_byte(USART_TypeDef *instance, uint8_t *byte)
{
    return false;
}
