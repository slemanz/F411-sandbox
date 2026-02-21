#include "driver_uart.h"
#include "driver_clock.h"
#include "driver_systick.h"

/*********************************************************************
 * Private Helper Functions
 *********************************************************************/

 static uint16_t compute_baud_div(uint32_t peripheralClk, uint32_t baudRate)
 {
    return (uint16_t)((peripheralClk + (baudRate/2U))/baudRate);
 }

 static UART_Error_e UART_CheckErrors(UART_RegDef_t *pUARTx)
 {
    uint32_t sr = pUARTx->SR;
    UART_Error_e error = UART_OK;

    if((sr & (1U << UART_SR_ORE)) != 0U)
    {
        error = UART_ERROR_ORE;
        (void)pUARTx->DR;
    }else if((sr & (1U << UART_SR_FE)) != 0U)
    {
        error = UART_ERROR_FE;
        (void)pUARTx->DR;
    }
    else if ((sr & (1U << UART_SR_NE)) != 0U)
    {
        error = UART_ERROR_NE;
        (void)pUARTx->DR;
    }
    else if ((sr & (1U << UART_SR_PE)) != 0U)
    {
        error = UART_ERROR_PE;
        (void)pUARTx->DR;
    }

    return error;
 }

static void configure_cr1(UART_RegDef_t *pUARTx, const UART_Config_t *pUARTConfig)
{
    uint32_t cr1 = 0U;

    switch(pUARTConfig->UART_Mode)
    {
        case UART_MODE_ONLY_RX: cr1 |= (1U << UART_CR1_RE);                         break;
        case UART_MODE_ONLY_TX: cr1 |= (1U << UART_CR1_TE);                         break;
        case UART_MODE_TXRX:    cr1 |= (1U << UART_CR1_RE) | (1U << UART_CR1_TE);   break;
        default:                                                                    break;
    }

    cr1 |= (pUARTConfig->UART_WordLength << UART_CR1_M);

    if(pUARTConfig->UART_ParityControl == UART_PARITY_EN_EVEN)
    {
        cr1 |= (1U << UART_CR1_PCE);
    }
    else if (pUARTConfig->UART_ParityControl == UART_PARITY_EN_ODD)
    {
        cr1 |= (1U << UART_CR1_PCE) | (1U << UART_CR1_PS);
    }

    pUARTx->CR1 = cr1;
}

static void configure_cr3(UART_RegDef_t *pUARTx, const UART_Config_t *pUARTConfig)
{
    uint32_t cr3 = 0U;

    switch (pUARTConfig->UART_HWFlowControl)
    {
        case UART_HW_FLOW_CTRL_CTS:     cr3 |= (1U << UART_CR3_CTSE);                           break;
        case UART_HW_FLOW_CTRL_RTS:     cr3 |= (1U << UART_CR3_RTSE);                           break;
        case UART_HW_FLOW_CTRL_CTS_RTS: cr3 |= (1U << UART_CR3_CTSE) | (1U << UART_CR3_RTSE);   break;
        default:                                                                                break;
    }

    pUARTx->CR3 = cr3;
}

/*********************************************************************
 * Public API Implementations
 *********************************************************************/