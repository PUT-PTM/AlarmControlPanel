#include "main.hpp"

extern int button_click_counter;
void vLedTask1(void *args)
{
    auto leds = Leds({OrangeLed, GreenLed});
    vTaskDelay(100);
    while (7) {        
        leds.turn_on();
        vTaskDelay(100);
        leds.turn_off();
        vTaskDelay(100);
//  manual interrupt (manual context switch)       taskYIELD();
     }
}

void vLedTask2(void *args)
{
    auto leds = Leds({BlueLed, RedLed});
    while (7) {
        leds.turn_on();
        vTaskDelay(100);
        leds.turn_off();
        vTaskDelay(100);
//  manual interrupt (manual context switch)       taskYIELD();
    }
}

void vPrintButtonCounter(void *args) {
    static int last_seen_cnt = button_click_counter;
    while (7) {
        if (last_seen_cnt != button_click_counter)
        {
            debug("Button clicked %d times.\n", button_click_counter);
            last_seen_cnt = button_click_counter;
        }
        vTaskDelay(100);
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
    debug("=== System started! ===\n");
    
    // Configure button interrupt
    debug("Configuring interrupts...\n");
    Interrupts::EXTIInt::enable_int(GPIOA, {GPIO::Pin::P0}, Interrupts::Mode::FallingEdgeInterrupt, EXTI0_IRQn, 2, 0);

    // Stack size 150 can be easly exceeded when extending tasks
    debug("Creating tasks...\n");
    xTaskCreate(vLedTask1, "Leds1", 150, NULL, 3, NULL);
    xTaskCreate(vLedTask2, "Leds2", 150, NULL, 3, NULL);
    xTaskCreate(vPrintButtonCounter, "Print Button Cnt", 250, NULL, 3, NULL);
    debug("Done! The rest is in TaskScheduler. Cya.\n");
    vTaskStartScheduler();

    return 0;
}
