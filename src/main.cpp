#include "main.hpp"

void vDebugInterrupt(void *args)
{
    debug("Interrupt Debug: %s\n", (char*)args);
    vTaskDelete(NULL);
}

void InterruptDebug(char *message)
{
    static char buffer[256];
    sprintf(buffer, "%s", message);
    xTaskCreate(vDebugInterrupt, "InterruptDebug", 3000, buffer, 3, NULL);
}

void* operator new  (std::size_t n)
{
    return pvPortMalloc(n);
}
void* operator new[](std::size_t n) {return pvPortMalloc(n);}
void  operator delete  (void* p) {vPortFree(p);}
void  operator delete[](void* p) {vPortFree(p);}

PIR::PIRManager *pPirManager;
Leds led1({GPIO::Pin::P11});
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

    //debug("Initializing button interrupt\n");
    //Interrupts::EXTIInt::enable_int(GPIOA, {GPIO::Pin::P0}, Interrupts::Mode::FallingEdgeInterrupt, EXTI0_IRQn, 2, 0);
    //debug("Initializing PIR interrupt\n");
    //Interrupts::EXTIInt::enable_int(GPIOC, {GPIO::Pin::P1}, Interrupts::Mode::RisingEdgeInterrupt, EXTI1_IRQn, 2, 0);

    //PIR::PIRManager pirManager;
    //pPirManager = &pirManager;
    pPirManager = new PIR::PIRManager();
    debug("Starting task scheduler...\n");
    vTaskStartScheduler();

    vPortFree(pPirManager);

    while(true)
    {
        
    }

    return 0;
}

