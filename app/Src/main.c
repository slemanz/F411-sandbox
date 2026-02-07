#include "config.h"

#include "bsp/led.h"
#include "core/simple-timer.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "shared/pool.h"

#include "driver_i2c.h"

void DS3231_ReadTime(void)
{
    uint8_t writeBuffer[2];
    uint8_t readAddrBuffer[1];
    uint8_t rtcData[3]; // [0]=Seg, [1]=Min, [2]=Hor

    writeBuffer[0] = 0xD0; // 0x68 << 1 | 0
    writeBuffer[1] = 0x00; 
    
    I2C_MasterSendData(I2C1, writeBuffer, 2, I2C_ENABLE_SR);

    readAddrBuffer[0] = 0xD1; // 0x68 << 1 | 1
    I2C_MasterSendData(I2C1, readAddrBuffer, 1, I2C_ENABLE_SR);
    I2C_MasterReceiveData(I2C1, rtcData, 3, I2C_DISABLE_SR);

    // rtcData agora contém os valores BCD de segundos, minutos e horas.
}

int main(void)
{
    config_app();

    simple_timer_t timer_blinky;
    simple_timer_setup(&timer_blinky, 500, true);

    uprint("Init the board!\r\n");

    ledPtr_t led1 = led_getByUuid(1);
    ledPtr_t led2 = led_getByUuid(2);

    while(1)
    {
        if(simple_timer_has_elapsed(&timer_blinky))
        {
            led_toggle(led1);
            led_toggle(led2);
            DS3231_ReadTime();
        }

        cli_update();
    }
}
