#include "main.hpp"

int main()
{
    /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
    HAL_Init();


    /* Configure the system clock to 168 MHz */
    SystemClock_Config();

    while (1)
    {
        debug("Test\n");
        Leds::turn_on({Led::Orange, Led::Green});
        for (int i = 0; i <= 1000000; i++);
        Leds::turn_off({Led::Orange, Led::Green});
        for (int i = 0; i <= 1000000; i++);
    }
}