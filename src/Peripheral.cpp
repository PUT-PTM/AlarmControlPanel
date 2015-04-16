#include "Peripheral.hpp"

//GPIO
#include "GPIO.hpp"

namespace Peripheral
{
	void Screen::Initialize(GPIO::Pin RS, GPIO::Pin RW, GPIO::Pin E, GPIO::Pin D4, GPIO::Pin D5, GPIO::Pin D6, GPIO::Pin D7)
	{
		// GPIO Pins Initialization
        _RS = new GPIO::GPIOPins(GPIOA, {RS}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _RW = new GPIO::GPIOPins(GPIOA, {RW}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _E  = new GPIO::GPIOPins(GPIOA, {E},  GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _D4 = new GPIO::GPIOPins(GPIOA, {D4}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _D5 = new GPIO::GPIOPins(GPIOA, {D5}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _D6 = new GPIO::GPIOPins(GPIOA, {D6}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
        _D7 = new GPIO::GPIOPins(GPIOA, {D7}, GPIO::Mode::OutputPushPull, GPIO::Pull::NoPull, GPIO::Speed::Low);
    }

    void Screen::Write(bool RS, uint8_t data)
    {
        _E->set_state(true);

        _RS->set_state(RS);
        _RW->set_state(false); //Write mode

        _D4->set_state(data & 0b00010000); //16
        _D5->set_state(data & 0b00100000); //32
        _D6->set_state(data & 0b01000000); //64
        _D7->set_state(data & 0b10000000); //128

        _E->set_state(false);

        // Wait?
        //HAL_Delay(1);

        _E->set_state(true);

        _D4->set_state(data & 0b00000001); //1
        _D5->set_state(data & 0b00000010); //2
        _D6->set_state(data & 0b00000100); //4
        _D7->set_state(data & 0b00001000); //8

        _E->set_state(false);
    }

    uint8_t Screen::Read(bool RS)
    {
        uint8_t result = 0;

        _E->set_state(true);

        _RS->set_state(RS);
        _RW->set_state(true); //Read mode

        _E->set_state(false);

        result |= (_D4->get_state() * 0b1000);
        result |= (_D5->get_state() * 0b1000);
        result |= (_D6->get_state() * 0b1000);
        result |= (_D7->get_state() * 0b1000);

        // Wait ?
        //HAL_Delay(1);

        _E->set_state(true);

        _E->set_state(false);

        result |= _D4->get_state();
        result |= _D5->get_state();
        result |= _D6->get_state();
        result |= _D7->get_state();

        return result;
    }
}
