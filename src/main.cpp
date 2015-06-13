#include "main.hpp"

void* operator new  (std::size_t n) {return pvPortMalloc(n);}
void* operator new[](std::size_t n) {return pvPortMalloc(n);}
void  operator delete  (void* p) {vPortFree(p);}
void  operator delete[](void* p) {vPortFree(p);}

Screen::Interface *interface;
Screen::LCD *lcd;
PIR::PIRManager *pPirManager;

Leds led1({GPIO::Pin::P12});
Leds led2({GPIO::Pin::P13});
Leds led3({GPIO::Pin::P14});
Leds led4({GPIO::Pin::P15});

void vInitializeScreen(void *args)
{
    lcd = new Screen::LCD(GPIOE, GPIO::Pin::P1, GPIO::Pin::P3, GPIO::Pin::P5, GPIO::Pin::P0, GPIO::Pin::P2, GPIO::Pin::P4, GPIO::Pin::P6);
    Screen::Interface::interface = new Screen::Interface(lcd);

    Screen::Interface::interface->SetMode(Screen::Interface::Mode::Input);
    Screen::Interface::interface->AppendCharToInput('b');

    //xTaskCreate(Peripheral::Keyboard::KeyboardCheckTask, "KeyboardInteruptTask", 1000, NULL, 4, NULL);
    xTaskCreate(Screen::Interface::CheckForInterruptTask, "ScreenInterruptTask", 1000, NULL, 4, NULL);

    vTaskDelete(NULL);
}

void vStillWorking(void *args)
{
    while(true)
    {
        debug("Still working, lol\n");
        HAL_Delay(100);
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
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Configure the system clock to 168 MHz */
    SystemClock_Config();

    Peripheral::Keyboard::keyboard = new Peripheral::Keyboard(GPIOE, GPIO::Pin::P11, GPIO::Pin::P12, GPIO::Pin::P13, GPIO::Pin::P14, GPIO::Pin::P7, GPIO::Pin::P8, GPIO::Pin::P9, GPIO::Pin::P10);

    debug("Starting task scheduler...\n");
    xTaskCreate(vInitializeScreen, "ScreenInit", 1000, NULL, 4, NULL);
    
    vTaskStartScheduler();

    while(true)
    {

    }

    return 0;
}

