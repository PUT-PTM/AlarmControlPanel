#include "main.hpp"

void vLedTask1(void *args)
{
    auto leds = Leds({OrangeLed, GreenLed});
    for (int i = 0; i <= 1000000; i++);
    while (7) {        
        leds.turn_on();
        for (int i = 0; i <= 1000000; i++);
        leds.turn_off();
        for (int i = 0; i <= 1000000; i++);
//  manual interrupt (manual context switch)       taskYIELD();
     }
}

void vLedTask2(void *args)
{
    auto leds = Leds({BlueLed, RedLed});
    while (7) {
        leds.turn_on();
        for (int i = 0; i <= 1000000; i++);
        leds.turn_off();
        for (int i = 0; i <= 1000000; i++);
//  manual interrupt (manual context switch)       taskYIELD();
    }
}

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
    
    // Stack size 100 can be easly exceeded when extending tasks
    xTaskCreate(vLedTask1, "Leds1", 100, NULL, 3, NULL);
    xTaskCreate(vLedTask2, "Leds2", 100, NULL, 3, NULL);
    vTaskStartScheduler();

    return 0;
}
