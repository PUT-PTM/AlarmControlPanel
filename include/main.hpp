#ifndef MAIN_H
#define MAIN_H

char* itoa(int value, char* result, int base);

#include <functional>
#include <string>
#include <unordered_map>

#include "system_config.hpp"
#include "debug.h"
#include "Leds.hpp"
#include "interrupts.hpp"

#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "enc28j60.h"

#include "server.hpp"
#include "request.hpp"
#include "response.hpp"
#include "constants.hpp"
#include "file_resources.hpp"

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



extern "C" {
    // buffer allocation declaration
    void vReleaseNetworkBufferAndDescriptor( xNetworkBufferDescriptor_t * const pxNetworkBuffer );
    xNetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks );
}

#endif /* MAIN_H */
