#include "interface/interface.h"
#include "interface_instances.h"
#include "interface_defines.h"


Comm_Interface_t *Comm_ProtocolGet(uint8_t type)
{
    switch (type)
    {
        case INTERFACE_PROTOCOL_UART2: return (Comm_Interface_t*)&uart2_protocol;
        case INTERFACE_PROTOCOL_I2C1: return (Comm_Interface_t*)&i2c1_protocol;
        default: return NULL;
    }
}