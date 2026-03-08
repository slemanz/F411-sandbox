#include "interface/interface.h"
#include "interface_defines.h"
#include "driver_timer.h"
#include "driver_gpio.h"
#include "driver_clock.h"

/* ------------------------------------------------------------------ */
/*  Internal type                                                      */
/* ------------------------------------------------------------------ */

typedef struct
{
    void (*init)    (void);
    void (*set_duty)(float duty_percent);
    void (*deinit)  (void);
} pwm_instance_t;

/* ------------------------------------------------------------------ */
/*  PWM0 — PA5, TIM2 CH1                                              */
/* ------------------------------------------------------------------ */

#define PWM0_FREQ_HZ    1000u
#define PWM0_RESOLUTION 1000u
#define PWM0_PRESCALER  TIM_PWM_CALC_PRESCALER(16000000u, PWM0_FREQ_HZ, PWM0_RESOLUTION)
#define PWM0_PERIOD     (PWM0_RESOLUTION - 1u)

static uint8_t s_pwm0_init = 0;

static TIM_Config_t s_pwm0_cfg = {
    .pTIMx     = TIM2,
    .prescaler = PWM0_PRESCALER,
    .period    = PWM0_PERIOD,
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
    TIM_PWM_Init(&s_pwm0_cfg);
    TIM_PWM_SetDuty(&s_pwm0_cfg, TIM_CHANNEL_1, 0.0f);
    TIM_Start(TIM2);
    s_pwm0_init = 1u;
}


static void pwm0_set_duty(float duty_percent)
{
    if (!s_pwm0_init) pwm0_init();
    TIM_PWM_SetDuty(&s_pwm0_cfg, TIM_CHANNEL_1, duty_percent);
}

static void pwm0_deinit(void)
{
    TIM_Stop(TIM2);
    s_pwm0_init = 0u;
}

/* ------------------------------------------------------------------ */
/*  Dispatch table                                                     */
/* ------------------------------------------------------------------ */

static const pwm_instance_t s_pwm_table[] = {
    [INTERFACE_PWM_0] = {
        .init     = pwm0_init,
        .set_duty = pwm0_set_duty,
        .deinit   = pwm0_deinit,
    },
};

#define PWM_COUNT ((uint8_t)(sizeof(s_pwm_table) / sizeof(s_pwm_table[0])))

static const pwm_instance_t *pwm_dispatch(uint8_t instance_id)
{
    if (instance_id >= PWM_COUNT) return NULL;
    return &s_pwm_table[instance_id];
}

/* ================================================================== */
/*  Public functions declared in interface.h                          */
/* ================================================================== */

void PWM_init(uint8_t instance_id)
{
    const pwm_instance_t *p = pwm_dispatch(instance_id);
    if (p && p->init) p->init();
}

void PWM_set_duty(uint8_t instance_id, float duty_percent)
{
    const pwm_instance_t *p = pwm_dispatch(instance_id);
    if (p && p->set_duty) p->set_duty(duty_percent);
}

void PWM_deinit(uint8_t instance_id)
{
    const pwm_instance_t *p = pwm_dispatch(instance_id);
    if (p && p->deinit) p->deinit();
}