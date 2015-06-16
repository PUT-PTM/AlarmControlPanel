#include "main.hpp"
/*
#include "GPIO.hpp"
#include "handlers.hpp"
#include "Leds.hpp"
#include "ControlPanel.hpp"
*/
int button_click_counter = 0;

void HttpserverTask( void *pvParameters );

extern "C" {
    void SysTick_Handler(void)
    {
        HAL_IncTick();
        xPortSysTickHandler();
    }

    void EXTI0_IRQHandler()
    {
        if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
            static uint32_t tickstart = 0;
            if((HAL_GetTick() - tickstart) > 200) {
                debug("Software int...");
                EXTI2_IRQHandler();
                tickstart = HAL_GetTick();
            }
        }
    }

    void EXTI9_5_IRQHandler(void)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
    }

    void EXTI15_10_IRQHandler(void)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
    }

    void EXTI1_IRQHandler(void)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    }

    void HAL_GPIO_EXTI_Callback(uint16_t pin)
    {
        GPIO::Pin gpiopin = static_cast<GPIO::Pin>(pin);
        if( gpiopin == ControlPanel::keyboard->GetRowPin(0) ||
            gpiopin == ControlPanel::keyboard->GetRowPin(1) ||
            gpiopin == ControlPanel::keyboard->GetRowPin(2) ||
            gpiopin == ControlPanel::keyboard->GetRowPin(3))
        {
            ControlPanel::keyboard->InterruptHandler(gpiopin);
            return;
        }

        switch(pin)
        {
            case GPIO_PIN_1:
                EXTI1_IRQHandlerCpp();
                break;
            case GPIO_PIN_6:
                //ControlPanel::pirManager->InterruptHandler(1);
                ControlPanel::pirManager->InterruptHandler(1);
                break;
            case GPIO_PIN_7:
                ControlPanel::pirManager->InterruptHandler(2);
                break;
            case GPIO_PIN_8:
                ControlPanel::pirManager->InterruptHandler(3);
                break;
            case GPIO_PIN_9:
                ControlPanel::pirManager->InterruptHandler(4);
                break;
        }

    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
    debug("FATAL EXCEPTION: Stack overflow\n");
    exit(EXIT_FAILURE);
}

void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier) {
    switch( eStatus )
    {
        case eSuccess    :
            /* A valid ping reply has been received.  Post the sequence number
               on the queue that is read by the vSendPing() function below.  Do
               not wait more than 10ms trying to send the message if it cannot be
               sent immediately because this function is called from the TCP/IP
               RTOS task - blocking in this function will block the TCP/IP RTOS task. */
            debug("vAppPingReply: Received ping reply.\n");
            break;

        case eInvalidChecksum :
        case eInvalidData :
            /* A reply was received but it was not valid. */
            break;
    }
}
void prvPingTask(void *pvParameters);
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent) {
    if( eNetworkEvent == eNetworkUp )
    {
        debug("vApplicationIP: network up.\n");

        xTaskCreate(prvPingTask, "pingtask", 1000, NULL, 3, NULL);
        xTaskCreate(DateTime::initialize, "InitDateTime", 1000, NULL, 3, NULL);

        if (xTaskCreate(HttpserverTask, "Httpserver", 12000, NULL, 3, NULL) != pdPASS)
            debug("!! Creation of Httpserver task failed.\n");
    } else if ( eNetworkEvent == eNetworkDown) {
        debug("vApplicationIP: network down.\n");
    }
}

void EXTI1_IRQHandlerCpp()
{
    static Leds diody({GreenLed});
    static uint32_t tickstart = 0;
    static int click_counter = 0;
    if((HAL_GetTick() - tickstart) > 50) {
        click_counter++;
        diody.set_state(click_counter % 2 == 0);
        tickstart = HAL_GetTick();
    }
}

