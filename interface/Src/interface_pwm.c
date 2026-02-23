#include "interface/interface.h"
#include "interface_instances.h"
#include "interface_defines.h"

#include "driver_timer.h"
#include "driver_gpio.h"
#include "driver_clock.h"

#define PWM0_FREQ_HZ        1000U
#define PWM0_RESOLUTION     1000U
#define PWM0_PRESCALER      TIM_PWM_CALC_PRESCALER(16000000U, PWM0_FREQ_HZ, PWM0_RESOLUTION)
#define PWM0_PERIOD         (PWM0_RESOLUTION - 1U)

static uint8_t pwm0_is_init = 0U;

static TIM_Config_t pwm0_config = {
    .pTIMx = TIM2,
    .prescaler  = PWM0_PRESCALER,
    .period     = PWM0_PERIOD
};

static void pwm0_init(void)
{
    GPIO_PinConfig_t pin = {
        .pGPIOx              = GPIOA,
        .GPIO_PinNumber      = GPIO_PIN_NO_5,
        .GPIO_PinMode        = GPIO_MODE_ALTFN,
        .GPIO_PinSpeed       = GPIO_SPEED_HIGH,
        .GPIO_PinOPType      = GPIO_OP_TYPE_PP,
        .GPIO_PinPuPdControl = GPIO_NO_PUPD,
        .GPIO_PinAltFunMode  = PA5_ALTFN_TIM2_CH1,
    };
    GPIO_Init(&pin);

    TIM_PWM_Init(&pwm0_config);
    TIM_PWM_SetDuty(&pwm0_config, TIM_CHANNEL_1, 0.0f);
    TIM_Start(TIM2);

    pwm0_is_init = 1U;
}

static void pwm0_set_duty(float dutyPercent)
{
    if (!pwm0_is_init) pwm0_init();
    TIM_PWM_SetDuty(&pwm0_config, TIM_CHANNEL_1, dutyPercent);
}

static void pwm0_deinit(void)
{
    TIM_Stop(TIM2);
    pwm0_is_init = 0U;
}

/************************************************************
*                    INSTANCE                               *
*************************************************************/

const PWM_Interface_t pwm0_out = {
    .init     = pwm0_init,
    .set_duty = pwm0_set_duty,
    .deinit   = pwm0_deinit,
};

/************************************************************
*                       GET                                 *
*************************************************************/

PWM_Interface_t *PWM_Interface_get(uint8_t instance)
{
    switch (instance)
    {
        case INTERFACE_PWM_0: return (PWM_Interface_t *)&pwm0_out;
        default:              return NULL;
    }
}
