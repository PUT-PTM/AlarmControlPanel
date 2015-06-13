#include "main.hpp"

void* operator new  (std::size_t n) {return pvPortMalloc(n);}
void* operator new[](std::size_t n) {return pvPortMalloc(n);}
void  operator delete  (void* p) {vPortFree(p);}
void  operator delete[](void* p) {vPortFree(p);}

Leds led1({GPIO::Pin::P12});
Leds led2({GPIO::Pin::P13});
Leds led3({GPIO::Pin::P14});
Leds led4({GPIO::Pin::P15});

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
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Configure the system clock to 168 MHz */
    SystemClock_Config();
    debug("Creating tasks...\n");
    xTaskCreate(ControlPanel::InitializeTask, "InitAll", 3000, NULL, 4, NULL);

    debug("Starting task scheduler...\n");
    
    vTaskStartScheduler();

    while(true)
    {

    }

    return 0;
}

