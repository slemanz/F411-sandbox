#include "interface/interface.h"
#include "interface_instances.h"

#include "driver_i2c.h"
#include "driver_gpio.h"

static uint8_t  i2c1_is_init = 0;

void i2c1_protocol_init(void)
{
    // pb6 - I2C1_SCL
    // pb7 - I2C1_SDA
    GPIO_PinConfig_t I2C_pin;
    I2C_pin.pGPIOx = GPIOB;
    I2C_pin.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2C_pin.GPIO_PinPuPdControl = GPIO_PIN_PU;
    I2C_pin.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    I2C_pin.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2C_pin.GPIO_PinAltFunMode = GPIO_PIN_ALTFN_4;

    I2C_pin.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2C_pin);

    I2C_pin.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2C_pin);

    I2C_Config_t I2C_config;
    I2C_config.pI2Cx = I2C1;
    I2C_config.I2C_DeviceAddress = 0x65;
    I2C_config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C_Init(&I2C_config);

    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_ManageAcking(I2C1, ENABLE);

    i2c1_is_init = 1;
}


void i2c1_protocol_send(uint8_t *data, uint32_t Len)
{
    if(!i2c1_is_init) i2c1_protocol_init();

    if(Len == 0) return;

    I2C_GenereteStart(I2C1);

    if(Len == 1)
    {
        I2C_SendAddress(I2C1, 0x68, I2C_SEND_READ);
    }else
    {
        I2C_SendAddress(I2C1, data[0], I2C_SEND_WRITE);
        I2C_Send(I2C1, &data[1], (Len - 1));
        I2C_GenereteStop(I2C1);
        I2C_WaitBusy(I2C1);
    }
}

uint8_t i2c1_protocol_receive(uint8_t *buffer, uint32_t Len)
{
    if(!i2c1_is_init) i2c1_protocol_init();

    if(Len == 0)
    {
        I2C_GenereteStop(I2C1);
        return 0;
    }

    I2C_Receive(I2C1, buffer, Len);
    I2C_GenereteStop(I2C1);

    return Len;
}

/************************************************************
*                     WRAPPER                               *
*************************************************************/

const Comm_Interface_t i2c1_protocol = {
    .init = i2c1_protocol_init,
    .send = i2c1_protocol_send,
    .receive = i2c1_protocol_receive,
    .data_available = NULL,
    .deinit = NULL
};