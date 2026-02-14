#include "config.h"

#include "bsp/led.h"
#include "core/simple-timer.h"
#include "core/cli.h"
#include "core/uprint.h"
#include "shared/pool.h"

#include "driver_i2c.h"

uint8_t writeBuffer[2];
uint8_t rtcData[3]; // [0]=Seg, [1]=Min, [2]=Hor

void DS3231_ReadTime(void)
{
    writeBuffer[0] = 0xD0; // 0x68 << 1 | 0
    writeBuffer[1] = 0x00; 
    
    I2C_GenereteStart(I2C1);
    I2C_SendAddress(I2C1, 0x68, I2C_SEND_WRITE);
    I2C_Send(I2C1, &writeBuffer[1], 1);
    I2C_GenereteStop(I2C1);
    I2C_WaitBusy(I2C1);

    I2C_GenereteStart(I2C1);
    I2C_SendAddress(I2C1, 0x68, I2C_SEND_READ);
    I2C_Receive(I2C1, rtcData, 3);
    I2C_GenereteStop(I2C1);
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
