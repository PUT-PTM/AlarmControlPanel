#include "main.hpp"
#include "timers.h"

namespace PIR
{
    PIRSensor::PIRSensor(GPIO_TypeDef *peripheral, GPIO::Pin pin, int pirID, int sensitivity)
    :   ID(pirID),
        Pin(pin),
        GPIOPeripheral(peripheral),
        GPIOPin(peripheral, {pin}, GPIO::Mode::Input, GPIO::Pull::PullDown, GPIO::Speed::Low)
    {
        char timerName[16];
        sprintf(timerName, "pirTimer%d", ID);
        Timer = xTimerCreate(timerName, sensitivity / portTICK_PERIOD_MS, pdFALSE, (void*)pirID, PIRManager::TimerHandler);

        switch(Pin)
        {
            case GPIO::Pin::P0:
                irqnType = EXTI0_IRQn;
                break;
            case GPIO::Pin::P1:
                irqnType = EXTI1_IRQn;
                break;
            case GPIO::Pin::P2:
                irqnType = EXTI2_IRQn;
                break;
            case GPIO::Pin::P3:
                irqnType = EXTI3_IRQn;
                break;
            case GPIO::Pin::P4:
                irqnType = EXTI4_IRQn;
                break;
            case GPIO::Pin::P5:
            case GPIO::Pin::P6:
            case GPIO::Pin::P7:
            case GPIO::Pin::P8:
            case GPIO::Pin::P9:
                irqnType = EXTI9_5_IRQn;
                break;
            case GPIO::Pin::P10:
            case GPIO::Pin::P11:
            case GPIO::Pin::P12:
            case GPIO::Pin::P13:
            case GPIO::Pin::P14:
            case GPIO::Pin::P15:
                irqnType = EXTI15_10_IRQn;
                break;
            default:
                return;
        }

        InitInterrupt();
    }

    void PIRSensor::InitInterrupt()
    {
        Interrupts::EXTIInt::enable_int(GPIOPeripheral, {Pin}, Interrupts::Mode::RisingEdgeInterrupt, irqnType, 15, 15);
    }

    bool PIRSensor::GPIOState()
    {
        return GPIOPin.get_state();
    }

    PIRManager::PIRManager()
    :   _pir1(GPIOC, {GPIO::Pin::P6}, 1, 100),
        _pir2(GPIOC, {GPIO::Pin::P7}, 2, 100),
        _pir3(GPIOC, {GPIO::Pin::P8}, 3, 100)
        //_pir4(GPIOC, GPIO::Pin::P9, 1, 100)
    {
        
    }

    void PIRManager::InterruptHandler(int pirId)
    {
        PIRSensor *pir = nullptr;

        switch(pirId)
        {
            case 1:
                pir = &_pir1;
                break;
            case 2:
                pir = &_pir2;
                break;
            case 3:
                pir = &_pir3;
                break;
            case 4:
                //pir = &_pir4;
                break;
            default:
                return;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerPendFunctionCallFromISR(StartTimer, pir, 0, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    void PIRManager::StartTimer(void *pPirSensor, uint32_t whatever)
    {
        PIRSensor *pir = (PIRSensor*)pPirSensor;
        xTimerStart(pir->Timer, 0);
        led4.turn_on();
    }

    void PIRManager::TimerHandler(TimerHandle_t xTimer)
    {
        led4.turn_off();
        int timerId = (int)pvTimerGetTimerID(xTimer);
        PIRSensor *pir = nullptr;

        switch(timerId)
        {
            case 1:
                pir = &pPirManager->_pir1;
                break;
            case 2:
                pir = &pPirManager->_pir2;
                break;
            case 3:
                pir = &pPirManager->_pir3;
                break;
            default:
                return;
        }

        if(pPirManager->_pir1.GPIOState())
            led1.toggle_state();

        if(pPirManager->_pir2.GPIOState())
            led2.toggle_state();

        if(pPirManager->_pir3.GPIOState())
            led3.toggle_state();
    }
}
