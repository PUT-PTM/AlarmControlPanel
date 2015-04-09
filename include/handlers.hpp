#include <cstdlib>

#include "debug.hpp"
#include "FreeRTOS.h"
#include "task.h"


extern "C" {
     // some declaration of stm32cubef4 and FreeRTOS functions
     void HAL_IncTick(void);
     void xPortSysTickHandler( void );

    [[ noreturn ]] void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
}
