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
        keyboard = this;
    }

    void Keyboard::SetRowMode(GPIO::Mode mode)
    {
        HAL_GPIO_DeInit(_peripheral, _R0.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _R1.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _R2.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _R3.get_pins_bitmask().to_ulong());

        _R0 = GPIO::GPIOPins(_peripheral, {_R0Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _R1 = GPIO::GPIOPins(_peripheral, {_R1Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _R2 = GPIO::GPIOPins(_peripheral, {_R2Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _R3 = GPIO::GPIOPins(_peripheral, {_R3Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
    }

    void Keyboard::SetColumnMode(GPIO::Mode mode)
    {
        HAL_GPIO_DeInit(_peripheral, _C0.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _C1.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _C2.get_pins_bitmask().to_ulong());
        HAL_GPIO_DeInit(_peripheral, _C3.get_pins_bitmask().to_ulong());

        _C0 = GPIO::GPIOPins(_peripheral, {_C0Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _C1 = GPIO::GPIOPins(_peripheral, {_C1Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _C2 = GPIO::GPIOPins(_peripheral, {_C2Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
        _C3 = GPIO::GPIOPins(_peripheral, {_C3Pin}, mode, GPIO::Pull::PullDown, GPIO::Speed::Low);
    }

    void Keyboard::SetRowState(bool state)
    {
        _R0.set_state(state);
        _R1.set_state(state);
        _R2.set_state(state);
        _R3.set_state(state);
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

    void Keyboard::DisableInterrupt()
    {
        Interrupts::EXTIInt::disable_int(_peripheral, {_R0Pin}, Interrupts::EXTIInt::get_irqn_type(_R0Pin));
        Interrupts::EXTIInt::disable_int(_peripheral, {_R1Pin}, Interrupts::EXTIInt::get_irqn_type(_R1Pin));
        Interrupts::EXTIInt::disable_int(_peripheral, {_R2Pin}, Interrupts::EXTIInt::get_irqn_type(_R2Pin));
        Interrupts::EXTIInt::disable_int(_peripheral, {_R3Pin}, Interrupts::EXTIInt::get_irqn_type(_R3Pin));
    }

    void Keyboard::InterruptHandler(GPIO::Pin pin)
    {
        //_interrupt = true;

        if(HAL_GetTick() <= _lastPressTick + 50) return;
        _lastPressTick = HAL_GetTick();

        int row = -1, column = -1;

        keyboard->SetColumnState(false);
        while(true)
        {
            keyboard->_C0.set_state(true);
            row = keyboard->GetRow();
            keyboard->_C0.set_state(false);

            if(row != -1)
            {
                column = 0;
                break;
            }

            keyboard->_C1.set_state(true);
            row = keyboard->GetRow();
            keyboard->_C1.set_state(false);

            if(row != -1)
            {
                column = 1;
                break;
            }

            keyboard->_C2.set_state(true);
            row = keyboard->GetRow();
            keyboard->_C2.set_state(false);

            if(row != -1)
            {
                column = 2;
                break;
            }

            keyboard->_C3.set_state(true);
            row = keyboard->GetRow();
            keyboard->_C3.set_state(false);

            if(row != -1)
            {
                column = 3;
                break;
            }

            break;
        }
        keyboard->SetColumnState(true);
        debug("LastPressed: %d\n", row*4 + column);
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

/*
    void Keyboard::KeyboardCheckTask(void * args)
    {
        int row = -1;
        int column = -1;

        while(true)
        {
            while(!keyboard->_interrupt) {}
            //debug("Got interrupt!\n");

            //Disable interrupts
            keyboard->DisableInterrupt();

            //Read states from row pins first (Row - Input, Column - Output)
            //Column mode is already set to GPIO::Mode::OutputPushPull
            keyboard->SetRowMode(GPIO::Mode::Input);
            //keyboard->SetRowMode(GPIO::Mode::OutputPushPull);

            if(keyboard->_R0.get_state())
                row = 0;
            else if(keyboard->_R1.get_state())
                row = 1;
            else if(keyboard->_R2.get_state())
                row = 2;
            else if(keyboard->_R3.get_state())
                row = 3;

            if(row == -1)
            {
                //No row detected - no need to continue, but set everything back
                //Row modes will 'automatically' switch to interrupt mode
                keyboard->EnableInterrupt();
                keyboard->_interrupt = false;
                debug("No row detected.\n");
                continue;
            }

            //Row number is read, so now read from columns (Row - Output, Column - Input)
            //Firstly set zeroes to columns
            keyboard->SetColumnState(false);

            //Now set the modes
            keyboard->SetRowMode(GPIO::Mode::OutputPushPull);
            keyboard->SetColumnMode(GPIO::Mode::Input);

            //Then set ones on rows
            keyboard->SetRowState(true);

            //Now we can read from columns
            if(keyboard->_C0.get_state())
                column = 0;
            if(keyboard->_C1.get_state())
                column = 1;
            if(keyboard->_C2.get_state())
                column = 2;
            if(keyboard->_C3.get_state())
                column = 3;

            //We are already past doing the hard work, so continue even if column is equal to -1
            //Now that we've read everything, set columns back to Output and rows back to Input
            //Firstly set zeroes on rows
            keyboard->SetRowState(false);

            //Now set the modes
            //No need to set mode on rows - interrupts will override that afterall
            keyboard->SetColumnMode(GPIO::Mode::OutputPushPull);

            //Then set ones on columns
            keyboard->SetColumnState(true);

            //Finally, enable interrupts
            keyboard->EnableInterrupt();
            keyboard->_interrupt = false;

            debug("Row: %d\nColumn: %d\nButton: %d\n", row, column, row*4+column);

            row = -1;
            column = -1;
        }
    }
     */

    int Keyboard::GetRow()
    {
        if(_R0.get_state())
            return 0;
        else if(_R1.get_state())
            return 1;
        else if(_R2.get_state())
            return 2;
        else if(_R3.get_state())
            return 3;

        return -1;
    }

    void Keyboard::KeyboardCheckTask(void * args)
    {
        int row = -1;
        int column = -1;

        while(true)
        {
            if(keyboard == 0) continue;
            while(!keyboard->_interrupt) {}
            
            keyboard->SetColumnState(false);
            while(true)
            {
                keyboard->_C0.set_state(true);
                row = keyboard->GetRow();
                keyboard->_C0.set_state(false);

                if(row != -1)
                {
                    column = 0;
                    break;
                }

                keyboard->_C1.set_state(true);
                row = keyboard->GetRow();
                keyboard->_C1.set_state(false);

                if(row != -1)
                {
                    column = 1;
                    break;
                }

                keyboard->_C2.set_state(true);
                row = keyboard->GetRow();
                keyboard->_C2.set_state(false);

                if(row != -1)
                {
                    column = 2;
                    break;
                }

                keyboard->_C3.set_state(true);
                row = keyboard->GetRow();
                keyboard->_C3.set_state(false);

                if(row != -1)
                {
                    column = 3;
                    break;
                }

                break;
            }
            keyboard->SetColumnState(true);

            debug("Keyboard interrupt.\n");
            debug("Button pressed: R:%d C:%d (%d)\n", row, column, row*4+column);

            keyboard->_interrupt = false;
        }
    }

    Keyboard * Keyboard::keyboard = 0;
}
