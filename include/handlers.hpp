#include <stdexcept>
#include "debug.hpp"

#include "FreeRTOS.h"
#include "task.h"

extern "C" {
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
}