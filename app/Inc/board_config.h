#ifndef INC_BOARD_CONFIG_H_
#define INC_BOARD_CONFIG_H_

#include "interface_defines.h"

void config_app(void);
void config_interface(void);
void config_core(void);

/************************************************************
*              BOARD PIN MAPPING — STM32F411E               *
*************************************************************/

/* LEDs */
#define BOARD_LED_ONBOARD       INTERFACE_IO_0      /* PC13 - onboard LED (inverted) */
#define BOARD_LED_RED           INTERFACE_IO_2      /* PB3  */
#define BOARD_LED_YELLOW        INTERFACE_IO_3      /* PB4  */
#define BOARD_LED_GREEN         INTERFACE_IO_4      /* PB5  */

/* Buttons */
#define BOARD_BUTTON_USER       INTERFACE_IO_5      /* PA0  */

/* PWM Outputs */
#define BOARD_PWM_OUTPUT1       INTERFACE_PWM_0     /* PA5 - TIM2 CH1 */

/* ADC Channels */
#define BOARD_ADC_CHANNEL0      INTERFACE_ADC_0     /* PA1 - ADC1 CH1 */

/* Communication */
#define BOARD_COMM_SERIAL       INTERFACE_PROTOCOL_UART2
#define BOARD_COMM_I2C          INTERFACE_PROTOCOL_I2C1

/* BSP UUIDs — explicitamente definidos para evitar dependencia de ordem */
#define BOARD_UUID_LED_ONBOARD  0
#define BOARD_UUID_LED_RED      1
#define BOARD_UUID_LED_YELLOW   2
#define BOARD_UUID_LED_GREEN    3
#define BOARD_UUID_BUTTON_USER  4
#define BOARD_UUID_OUTPUT1      5

#endif /* INC_BOARD_CONFIG_H_ */