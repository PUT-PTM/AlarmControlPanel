#ifndef PERIPHERAL_HPP
#define PERIPHERAL_HPP

#include "GPIO.hpp"

namespace Peripheral
{
	class Screen
	{
    private:
        GPIO::GPIOPins * _RS;   // Registry Set ( 0 = Instruction Register (W) Busy Flag: address counter (R), 1 = Data Register (RW) )
        GPIO::GPIOPins * _RW;   // Read/Write ( 0 = W, 1 = R)
        GPIO::GPIOPins * _E;    // Clock (?), screen starts reading on falling edge

        GPIO::GPIOPins * _D4;
        GPIO::GPIOPins * _D5;
        GPIO::GPIOPins * _D6;
        GPIO::GPIOPins * _D7;

        void SetPins(bool RS, bool RW, bool D4, bool D5, bool D6, bool D7)
        {
            _RS->set_state(RS);
            _RW->set_state(RW);
            _D4->set_state(D4);
            _D5->set_state(D5);
            _D6->set_state(D6);
            _D7->set_state(D7);
        }
	public:
		void Initialize(GPIO::Pin RS = GPIO::Pin::P0, GPIO::Pin RW = GPIO::Pin::P2, GPIO::Pin E = GPIO::Pin::P4, GPIO::Pin D4 = GPIO::Pin::P1, GPIO::Pin D5 = GPIO::Pin::P3, GPIO::Pin D6 = GPIO::Pin::P5, GPIO::Pin D7 = GPIO::Pin::P7);

        void Write(bool RS, uint8_t data);
        uint8_t Read(bool RS);
	};
}

#endif // PERIPHERAL_HPP
