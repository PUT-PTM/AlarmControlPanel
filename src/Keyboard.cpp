#include "main.hpp"

namespace Peripheral
{
    Keyboard::Keyboard(GPIO_TypeDef *peripheral, GPIO::Pin R0, GPIO::Pin R1, GPIO::Pin R2, GPIO::Pin R3, GPIO::Pin C0, GPIO::Pin C1, GPIO::Pin C2, GPIO::Pin C3)
    :   _peripheral(peripheral),
        _R0Pin(R0),
        _R1Pin(R1),
        _R2Pin(R2),
        _R3Pin(R3),
        _C0Pin(C0),
        _C1Pin(C1),
        _C2Pin(C2),
        _C3Pin(C3),
        _R0(peripheral, {R0}, GPIO::Mode::Input, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _R1(peripheral, {R1}, GPIO::Mode::Input, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _R2(peripheral, {R2}, GPIO::Mode::Input, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _R3(peripheral, {R3}, GPIO::Mode::Input, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _C0(peripheral, {C0}, GPIO::Mode::OutputPushPull, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _C1(peripheral, {C1}, GPIO::Mode::OutputPushPull, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _C2(peripheral, {C2}, GPIO::Mode::OutputPushPull, GPIO::Pull::PullDown, GPIO::Speed::Low),
        _C3(peripheral, {C3}, GPIO::Mode::OutputPushPull, GPIO::Pull::PullDown, GPIO::Speed::Low)
    {
        SetColumnState(true);
        EnableInterrupt();
    }

    void Keyboard::SetColumnState(bool state)
    {
        _C0.set_state(state);
        _C1.set_state(state);
        _C2.set_state(state);
        _C3.set_state(state);
    }

    void Keyboard::EnableInterrupt()
    {
        Interrupts::EXTIInt::enable_int(_peripheral, {_R0Pin}, Interrupts::Mode::RisingEdgeInterrupt, Interrupts::EXTIInt::get_irqn_type(_R0Pin), 15, 15);
        Interrupts::EXTIInt::enable_int(_peripheral, {_R1Pin}, Interrupts::Mode::RisingEdgeInterrupt, Interrupts::EXTIInt::get_irqn_type(_R1Pin), 15, 15);
        Interrupts::EXTIInt::enable_int(_peripheral, {_R2Pin}, Interrupts::Mode::RisingEdgeInterrupt, Interrupts::EXTIInt::get_irqn_type(_R2Pin), 15, 15);
        Interrupts::EXTIInt::enable_int(_peripheral, {_R3Pin}, Interrupts::Mode::RisingEdgeInterrupt, Interrupts::EXTIInt::get_irqn_type(_R3Pin), 15, 15);
    }

    void Keyboard::InterruptHandler(GPIO::Pin pin)
    {
        //_interrupt = true;

        if(HAL_GetTick() <= _lastPressTick + 50) return;
        _lastPressTick = HAL_GetTick();

        int row = -1, column = -1;

        SetColumnState(false);
        while(true)
        {
            _C0.set_state(true);
            row = GetRow();
            _C0.set_state(false);

            if(row != -1)
            {
                column = 0;
                break;
            }

            _C1.set_state(true);
            row = GetRow();
            _C1.set_state(false);

            if(row != -1)
            {
                column = 1;
                break;
            }

            _C2.set_state(true);
            row = GetRow();
            _C2.set_state(false);

            if(row != -1)
            {
                column = 2;
                break;
            }

            _C3.set_state(true);
            row = GetRow();
            _C3.set_state(false);

            if(row != -1)
            {
                column = 3;
                break;
            }

            break;
        }
        SetColumnState(true);
        debug("LastPressed: %d\n", row*4 + column);

        if(GetButton(row, column) != Button::None)
            ControlPanel::KeyboardInterrupt(GetButton(row, column));
    }

    GPIO::Pin Keyboard::GetRowPin(uint8_t rowNumber)
    {
        switch(rowNumber)
        {
            case 0: return _R0Pin;
            case 1: return _R1Pin;
            case 2: return _R2Pin;
            case 3: return _R3Pin;
            default: return _R0Pin;
        }
    }

    int Keyboard::GetRow()
    {
        if(_R1.get_state())
            return 1;
        else if(_R0.get_state())
            return 0;
        else if(_R2.get_state())
            return 2;
        else if(_R3.get_state())
            return 3;

        return -1;
    }

    Keyboard::Button Keyboard::GetButton(int row, int column)
    {
        switch(row)
        {
            case 0:
                switch(column)
                {
                    case 0: return Button::B1;
                    case 1: return Button::B2;
                    case 2: return Button::B3;
                    case 3: return Button::BA;
                    default: return Button::None;
                }
            case 1:
                switch(column)
                {
                    case 0: return Button::B4;
                    case 1: return Button::B5;
                    case 2: return Button::B6;
                    case 3: return Button::BB;
                    default: return Button::None;
                }
            case 2:
                switch(column)
                {
                    case 0: return Button::B7;
                    case 1: return Button::B8;
                    case 2: return Button::B9;
                    case 3: return Button::BC;
                    default: return Button::None;
                }
            case 3:
                switch(column)
                {
                    case 0: return Button::BAsterisk;
                    case 1: return Button::B0;
                    case 2: return Button::BHash;
                    case 3: return Button::BD;
                    default: return Button::None;
                }
            default: return Button::None;
        }
    }
}
