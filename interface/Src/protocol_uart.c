#include "interface/interface.h"
#include "interface_instances.h"

#include "shared/ring-buffer.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "driver_interrupt.h"

// Ring buffer
#define SERIAL_BUFFER_SIZE     128

/************************************************************
*                       SCI2                                *
*************************************************************/

static ring_buffer_t rb_uart2 = {0U};
static uint8_t data_buffer_uart2[SERIAL_BUFFER_SIZE] = {0U};
static uint8_t uart2_is_init = 0;

void uart2_protocol_init(void)
{
    ring_buffer_setup(&rb_uart2, data_buffer_uart2, SERIAL_BUFFER_SIZE);

    GPIO_PinConfig_t uart_pin;
    uart_pin.pGPIOx = GPIOA;
    uart_pin.GPIO_PinNumber = GPIO_PIN_NO_3;
    uart_pin.GPIO_PinMode = GPIO_MODE_ALTFN;
    uart_pin.GPIO_PinSpeed = GPIO_SPEED_LOW;
    uart_pin.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    uart_pin.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    uart_pin.GPIO_PinAltFunMode = PA3_ALTFN_UART2_RX;
    GPIO_Init(&uart_pin);

    uart_pin.pGPIOx = GPIOA;
    uart_pin.GPIO_PinNumber = GPIO_PIN_NO_2;
    uart_pin.GPIO_PinAltFunMode = PA2_ALTFN_UART2_TX;
    GPIO_Init(&uart_pin);

    UART_Config_t uart_config;
    uart_config.pUARTx = UART2;
    uart_config.UART_Mode = UART_MODE_TXRX;
    uart_config.UART_Baud = UART_STD_BAUD_115200;
    uart_config.UART_NoOfStopBits = UART_STOPBITS_1;
    uart_config.UART_WordLength = UART_WORDLEN_8BITS;
    uart_config.UART_ParityControl = UART_PARITY_DISABLE;
    uart_config.UART_HWFlowControl = UART_HW_FLOW_CTRL_NONE;

    UART_Init(&uart_config);

    UART_InterruptConfig(UART2, UART_INTERRUPT_RXNEIE, ENABLE);
    interrupt_Config(IRQ_NO_UART2, ENABLE);
    UART_PeripheralControl(UART2, ENABLE);

    uart2_is_init = 1;
}


void uart2_protocol_send(uint8_t *data, uint32_t Len)
{
    if(!uart2_is_init) uart2_protocol_init();
    UART_write(UART2, data, Len);
}

uint8_t uart2_protocol_receive(uint8_t *buffer, uint32_t Len)
{
    if(!uart2_is_init) uart2_protocol_init();

    if(Len == 0)
    {
        return 0;
    }

    for(uint8_t bytes_read = 0; bytes_read < Len; bytes_read++)
    {
        if(!ring_buffer_read(&rb_uart2, &buffer[bytes_read])) return bytes_read;
    }

    return Len;
}

uint8_t uart2_protocol_data_available(void)
{
    if(!uart2_is_init) uart2_protocol_init();
    return !ring_buffer_empty(&rb_uart2);
}

void USART2_IRQHandler(void)
{
	//const bool overrun_occurred = UART_GetFlagStatus(UART2, UART_FLAG_ORE);
	//const bool received_data = UART_GetFlagStatus(UART2, UART_FLAG_RXNE);

	//if(received_data || overrun_occurred)
    uint8_t data = UART2->DR;
    ring_buffer_write(&rb_uart2, data);
}


/************************************************************
*                     WRAPPER                               *
*************************************************************/

const Comm_Interface_t uart2_protocol = {
    .init = uart2_protocol_init,
    .send = uart2_protocol_send,
    .receive = uart2_protocol_receive,
    .data_available = uart2_protocol_data_available,
    .deinit = NULL
};