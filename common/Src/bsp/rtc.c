#include "bsp/rtc.h"

uint8_t binary_to_bcd(uint8_t value)
{
    uint8_t m, n;
    uint8_t bcd;

    bcd = value;
    if(value >= 10)
    {
        m = value/10;
        n = value % 10;
        bcd = (uint8_t)((m << 4) | n);
    }

    return bcd;
}

uint8_t bcd_to_binary(uint8_t value)
{
    uint8_t m, n;
    uint8_t bin;

    m = (uint8_t)((value >> 4) * 10);
    n = value & (uint8_t)0x0F;

    bin = m + n;
    return bin;
}