#include "ttest.h"

//void test_sanity_suite(void);
void test_gpio_suite(void);
void test_systick_suite(void);
void test_uart_suite(void);

int main(void)
{
    ttest_begin();

    //test_sanity_suite();
    test_gpio_suite();
    test_systick_suite();
    test_uart_suite();

    ttest_end();
}