#include <cstdlib>

#include "debug.hpp"
#include "FreeRTOS.h"
#include "task.h"

extern "C" {
    [[ noreturn ]] void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
}