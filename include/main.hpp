#ifndef MAIN_H
#define MAIN_H

#include "system_config.hpp"
#include "debug.h"
#include "Leds.hpp"
#include "interrupts.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "handlers.hpp"
#include "Peripheral.hpp"
#include "timers.h"
#include "PIR.hpp"

#include "Screen.hpp"

extern PIR::PIRManager *pPirManager;
extern Leds led1;
extern Leds led2;
extern Leds led3;
extern Leds led4;
void InterruptDebug(char *message);

extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );

#endif /* MAIN_H */
