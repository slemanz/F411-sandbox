#include "interface/interface.h"
#include "interface_instances.h"
#include "interface_defines.h"

#include "driver_adc.h"
#include "driver_gpio.h"

/*
 * INTERFACE_ADC_0 -> PA1 -> ADC1 Channel 1
 *
 * PA1 is free on the current board; connect your potentiometer here.
 * Wiring: potentiometer wiper -> PA1, supply rails to 3.3V and GND.
 */

#define ADC0_GPIO_PORT      GPIOA
#define ADC0_GPIO_PIN       GPIO_PIN_NO_1
#define ADC0_CHANNEL        ADC_CHANNEL_1

static uint8_t adc0_is_init = 0U;

static ADC_Config_t adc0_config = {
    .pADCx             = ADC1,
    .ADC_Resolution    = ADC_RESOLUTION_12BIT,
    .ADC_SampleTime    = ADC_SAMPLETIME_480,
    .ADC_DataAlignment = ADC_ALIGN_RIGHT,
};

static void adc0_init(void)
{
    GPIO_PinConfig_t pin = {
        .pGPIOx              = ADC0_GPIO_PORT,
        .GPIO_PinNumber      = ADC0_GPIO_PIN,
        .GPIO_PinMode        = GPIO_MODE_ANALOG,
        .GPIO_PinSpeed       = GPIO_SPEED_LOW,
        .GPIO_PinOPType      = GPIO_OP_TYPE_PP,
        .GPIO_PinPuPdControl = GPIO_NO_PUPD,
        .GPIO_PinAltFunMode  = GPIO_PIN_ALTFN_0,
    };
    GPIO_Init(&pin);

    ADC_Init(&adc0_config);
    ADC_PeripheralControl(ADC1, ENABLE);

    adc0_is_init = 1U;
}

static uint16_t adc0_read(void)
{
    if (!adc0_is_init) adc0_init();

    uint16_t result = 0U;
    ADC_ReadChannel(ADC1, ADC0_CHANNEL, &result);
    return result;
}

/************************************************************
*                    INSTANCE                               *
*************************************************************/

const ADC_Interface_t adc0_ch = {
    .init   = adc0_init,
    .read   = adc0_read,
    .deinit = NULL,
};

/************************************************************
*                       GET                                 *
*************************************************************/

ADC_Interface_t *ADC_Interface_get(uint8_t channel)
{
    switch (channel)
    {
        case INTERFACE_ADC_0: return (ADC_Interface_t *)&adc0_ch;
        default:              return NULL;
    }
}