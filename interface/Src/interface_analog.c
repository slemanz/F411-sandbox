#include "interface/interface.h"
#include "interface_defines.h"
#include "driver_adc.h"
#include "driver_gpio.h"

/* ------------------------------------------------------------------ */
/*  Internal type                                                      */
/* ------------------------------------------------------------------ */

typedef struct
{
    void     (*init)  (void);
    uint16_t (*read)  (void);
    void     (*deinit)(void);
} adc_instance_t;

/* ------------------------------------------------------------------ */
/*  ADC0 — PA1, ADC1 channel 1                                        */
/* ------------------------------------------------------------------ */

static uint8_t s_adc0_init = 0u;

static ADC_Config_t s_adc0_cfg = {
    .pADCx             = ADC1,
    .ADC_Resolution    = ADC_RESOLUTION_12BIT,
    .ADC_SampleTime    = ADC_SAMPLETIME_480,
    .ADC_DataAlignment = ADC_ALIGN_RIGHT,
};

static void adc0_init(void)
{
    GPIO_PinConfig_t pin = {
        .pGPIOx              = GPIOA,
        .GPIO_PinNumber      = GPIO_PIN_NO_1,
        .GPIO_PinMode        = GPIO_MODE_ANALOG,
        .GPIO_PinSpeed       = GPIO_SPEED_LOW,
        .GPIO_PinOPType      = GPIO_OP_TYPE_PP,
        .GPIO_PinPuPdControl = GPIO_NO_PUPD,
        .GPIO_PinAltFunMode  = GPIO_PIN_ALTFN_0,
    };
    GPIO_Init(&pin);
    ADC_Init(&s_adc0_cfg);
    ADC_PeripheralControl(ADC1, ENABLE);
    s_adc0_init = 1u;
}

static uint16_t adc0_read(void)
{
    if (!s_adc0_init) adc0_init();
    uint16_t result = 0u;
    ADC_ReadChannel(ADC1, ADC_CHANNEL_1, &result);
    return result;
}

static void adc0_deinit(void)
{
    s_adc0_init = 0u;
}

/* ------------------------------------------------------------------ */
/*  Dispatch table                                                     */
/* ------------------------------------------------------------------ */

static const adc_instance_t s_adc_table[] = {
    [INTERFACE_ADC_0] = {
        .init   = adc0_init,
        .read   = adc0_read,
        .deinit = adc0_deinit,
    },
};

#define ADC_COUNT ((uint8_t)(sizeof(s_adc_table) / sizeof(s_adc_table[0])))

static const adc_instance_t *adc_dispatch(uint8_t channel_id)
{
    if (channel_id >= ADC_COUNT) return NULL;
    return &s_adc_table[channel_id];
}

/* ================================================================== */
/*  Public functions declared in interface.h                          */
/* ================================================================== */

void analog_init(uint8_t channel_id)
{
    const adc_instance_t *a = adc_dispatch(channel_id);
    if (a && a->init) a->init();
}

uint16_t analog_read(uint8_t channel_id)
{
    const adc_instance_t *a = adc_dispatch(channel_id);
    if (a && a->read) return a->read();
    return 0u;
}

void analog_deinit(uint8_t channel_id)
{
    const adc_instance_t *a = adc_dispatch(channel_id);
    if (a && a->deinit) a->deinit();
}