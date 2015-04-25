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

    debug("Initializing button interrupt\n");
    Interrupts::EXTIInt::enable_int(GPIOA, {GPIO::Pin::P0}, Interrupts::Mode::FallingEdgeInterrupt, EXTI0_IRQn, 2, 0);

    debug("Creating Interface object...\n");

    Screen::Interface interface;

    std::string menu[5] = { "Pos 0", "Pos 1", "Pos 2", "Pos 3", "Pos 4" };
    interface.SetMenu(menu, 5);
    interface.Redraw();

    debug("Done.\n");

    while(true)
    {
        debug("Loop\n");
        HAL_Delay(100);
        debug("    Loop\n");
        HAL_Delay(100);
    }

    return 0;
}

