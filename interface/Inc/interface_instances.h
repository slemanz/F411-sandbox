#ifndef INC_INTERFACE_INSTANCES_H_
#define INC_INTERFACE_INSTANCES_H_

#include "interface/interface.h"

/************************************************************
*                   COMM INSTANCES                          *
*************************************************************/

extern const Comm_Interface_t uart2_protocol;
extern const Comm_Interface_t i2c1_protocol;


/************************************************************
*                     IO INSTANCES                          *
*************************************************************/

/************************************************************
*                    ADC INSTANCES                          *
*************************************************************/

extern const ADC_Interface_t adc0_ch;

/************************************************************
*                    PWM INSTANCES                          *
*************************************************************/

extern const PWM_Interface_t pwm0_out;

#endif /* INC_INTERFACE_INSTANCES_H_ */