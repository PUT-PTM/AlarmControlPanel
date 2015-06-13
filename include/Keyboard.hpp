#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "GPIO.hpp"

namespace Peripheral
{
    class Keyboard
    {
    public:
        enum class Button : uint8_t
        {
            B0,
            B1,
            B2,
            B3,
            B4,
            B5,
            B6,
            B7,
            B8,
            B9,
            BAsterisk,
            BHash,
            BA,
            BB,
            BC,
            BD,
            None
        };

    private:
        GPIO_TypeDef *_peripheral;
        
        GPIO::Pin _R0Pin;
        GPIO::Pin _R1Pin;
        GPIO::Pin _R2Pin;
        GPIO::Pin _R3Pin;

        GPIO::Pin _C0Pin;
        GPIO::Pin _C1Pin;
        GPIO::Pin _C2Pin;
        GPIO::Pin _C3Pin;

        GPIO::GPIOPins _R0;
        GPIO::GPIOPins _R1;
        GPIO::GPIOPins _R2;
        GPIO::GPIOPins _R3;

        GPIO::GPIOPins _C0;
        GPIO::GPIOPins _C1;
        GPIO::GPIOPins _C2;
        GPIO::GPIOPins _C3;

        volatile bool _interrupt = false;
        volatile int _lastPressTick = 0;

        void EnableInterrupt();
        void SetColumnState(bool state);

        int GetRow();
        static Button GetButton(int row, int column);

    public:
        Keyboard(GPIO_TypeDef *peripheral, GPIO::Pin R0, GPIO::Pin R1, GPIO::Pin R2, GPIO::Pin R3, GPIO::Pin C0, GPIO::Pin C1, GPIO::Pin C2, GPIO::Pin C3);
        void InterruptHandler(GPIO::Pin pin);
        GPIO::Pin GetRowPin(uint8_t rowNumber);

        static void KeyboardCheckTask(void * args);
        static Keyboard * keyboard;
    };


}


#endif //KEYBOARD_HPP
