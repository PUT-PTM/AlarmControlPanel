#include "handlers.hpp"

int button_click_counter = 0;

extern "C" {
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
        debug("FATAL EXCEPTION: Stack overflow\n");
        exit(EXIT_FAILURE);
    }

    void SysTick_Handler(void)
    {
        HAL_IncTick();
        xPortSysTickHandler();
    }

    void EXTI0_IRQHandler(void)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    }

    void HAL_GPIO_EXTI_Callback(uint16_t pin)
    {
        EXTI0_IRQHandlerCpp();
    }
}

void EXTI0_IRQHandlerCpp()
{
    static uint32_t tickstart = 0;
    if((HAL_GetTick() - tickstart) > 50) {
        button_click_counter++;
        if (button_click_counter >= 5) {
            Interrupts::EXTIInt::disable_int(GPIOA, {GPIO::Pin::P0}, EXTI0_IRQn);
        }
        tickstart = HAL_GetTick();
    }
}
