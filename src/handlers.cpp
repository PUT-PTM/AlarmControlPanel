#include "handlers.hpp"

extern "C" {
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
        debug("FATAL EXCEPTION: Stack overflow\n");
        exit(EXIT_FAILURE);
    }
}