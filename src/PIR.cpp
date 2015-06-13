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
        Timer = xTimerCreate(timerName, sensitivity / portTICK_PERIOD_MS, pdFALSE,
                             (void*)pirID, PIRManager::TimerHandler);

        irqnType = Interrupts::EXTIInt::get_irqn_type(Pin);

        InitInterrupt();
    }

    void PIRSensor::InitInterrupt()
    {
        Interrupts::EXTIInt::enable_int(GPIOPeripheral, {Pin},
                                        Interrupts::Mode::RisingEdgeInterrupt, irqnType,
                                        15, 15);
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

        if(ControlPanel::pirManager->_pir1.GPIOState()) {
            led1.toggle_state();
            events_log.emplace_back(Event{"Movement detected", "Sensor 1", "12:00", 1});
        }

        if(ControlPanel::pirManager->_pir2.GPIOState()) {
            led2.toggle_state();
            events_log.emplace_back(Event{"Movement detected", "Sensor 2", "12:00", 1});
        }

        if(ControlPanel::pirManager->_pir3.GPIOState()) {
            led3.toggle_state();
            events_log.emplace_back(Event{"Movement detected", "Sensor 3", "12:00", 1});
        }
    }

    std::vector<Event> PIRManager::events_log;
}
