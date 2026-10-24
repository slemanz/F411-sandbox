#include "stm32f4xx.h"

static void delay(volatile uint32_t count)
{
    while(count--);
}

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER |= (1U << GPIO_MODER_MODER13_Pos);   /* PC13 output */

    while(1)
    {
        GPIOC->ODR ^= GPIO_ODR_OD13;
        delay(500000);
    }
}