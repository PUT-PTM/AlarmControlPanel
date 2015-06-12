#ifndef PIR_HPP
#define PIR_HPP

#include <vector>
#include <string>

#include "debug.h"

namespace PIR
{
    struct Event {
        std::string name;
        std::string source;
        std::string time;
        uint16_t duration;
    };

    class PIRSensor
    {
    public:
        int ID;
        GPIO::Pin Pin;
        GPIO_TypeDef *GPIOPeripheral;
        GPIO::GPIOPins GPIOPin;
        IRQn_Type irqnType;
        TimerHandle_t Timer;

        PIRSensor(GPIO_TypeDef *peripheral, GPIO::Pin pin, int pirID, int sensitivity);
        
        void InitInterrupt();
        bool GPIOState();
    };

    class PIRManager
    {
    private:
        PIRSensor _pir1;
        PIRSensor _pir2;
        PIRSensor _pir3;
        //PIRSensor _pir4;

    public:
        static std::vector<Event> events_log;

        PIRManager();

        void InterruptHandler(int pirId);
        static void StartTimer(void *pPirSensor, uint32_t whatever);
        static void TimerHandler(TimerHandle_t xTimer);
    };
}

#endif // PIR_HPP
