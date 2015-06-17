#include "main.hpp"

void* operator new  (std::size_t n) {return pvPortMalloc(n);}
void* operator new[](std::size_t n) {return pvPortMalloc(n);}
void  operator delete  (void* p) {vPortFree(p);}
void  operator delete[](void* p) {vPortFree(p);}



// TCP Stack - IP configuration
uint8_t ucMACAddress[ 6 ] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
static const uint8_t ucIPAddress[ 4 ] = { 192, 168, 0, 2 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 0, 100 };

// The following is the address of an OpenDNS server.
static const uint8_t ucDNSServerAddress[ 4 ] = { 208, 67, 222, 222 };

void prvInitEthernet( void *pvParameters );
void prvPingTask(void *pvParameters);

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

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    FreeRTOS_IPInit(ucIPAddress,
                    ucNetMask,
                    ucGatewayAddress,
                    ucDNSServerAddress,
                    ucMACAddress);

    debug("Creating tasks...\n");
    xTaskCreate(ControlPanel::InitializeTask, "InitAll", 3000, NULL, 3, NULL);

    debug("Starting task scheduler...\n");

    vTaskStartScheduler();

    while(true)
    {

    }

    return 0;
}


void prvPingTask(void *pvParameters)
{
    for(;;) {
        debug("Sending ping request...\n");
        FreeRTOS_SendPingRequest(FreeRTOS_inet_addr("192.168.0.1") , 8, 100 / portTICK_PERIOD_MS );
        vTaskDelay(2000);
    }
}


