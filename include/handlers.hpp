#include <cstdlib>

#include "interrupts.hpp"
#include "debug.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "task.h"


extern "C" {
     // some declaration of stm32cubef4 and FreeRTOS functions
     void HAL_IncTick(void);
     void xPortSysTickHandler( void );
     void EXTI0_IRQHandler(void);
     void EXTI2_IRQHandler(void);

    [[ noreturn ]] void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
}

void EXTI1_IRQHandlerCpp();
