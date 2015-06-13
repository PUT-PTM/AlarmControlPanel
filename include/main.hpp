#ifndef MAIN_H
#define MAIN_H

#include <functional>

#include "system_config.hpp"
#include "debug.h"
#include "Leds.hpp"
#include "interrupts.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "Peripheral.hpp"
#include "timers.h"
#include "PIR.hpp"
#include "Keyboard.hpp"
#include "Screen.hpp"
#include "Menus.hpp"
#include "ControlPanel.hpp"
#include "handlers.hpp"

extern Leds led1;
extern Leds led2;
extern Leds led3;
extern Leds led4;

extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );

#endif /* MAIN_H */
