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

extern const IO_Interface_t io0_pin;
extern const IO_Interface_t io1_pin;
extern const IO_Interface_t io2_pin;
extern const IO_Interface_t io3_pin;

#endif /* INC_INTERFACE_INSTANCES_H_ */