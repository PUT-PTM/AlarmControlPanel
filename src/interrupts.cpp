#include "interrupts.hpp"

namespace Interrupts {
    void EXTIInt::enable_int(GPIO_TypeDef *peripheral, std::initializer_list<GPIO::Pin> pins, Mode interrupt_mode,
            IRQn_Type irqn_type, int32_t PreemptionPriority, uint32_t SubPriority)
    {
        interrupt_list.emplace_back(peripheral, pins, static_cast<GPIO::Mode>(interrupt_mode), GPIO::Pull::PullDown, GPIO::Speed::Low);
        HAL_NVIC_SetPriority(irqn_type, PreemptionPriority, SubPriority);
        HAL_NVIC_EnableIRQ(irqn_type);
    }

    void EXTIInt::disable_int(GPIO_TypeDef *peripheral, std::initializer_list<GPIO::Pin> pins, IRQn_Type irqn_type)
    {
        std::bitset<16> pins_bitmask;
        for (const auto &pin : pins) {
            pins_bitmask |= std::bitset<16>(static_cast<uint16_t>(pin));
        }

        HAL_NVIC_DisableIRQ(irqn_type);
        
        /*
        auto list_iter = find_gpiopins(interrupt_list, peripheral, pins_bitmask);
        if (list_iter == interrupt_list.end()) {
            debug("Interrupt GPIO: %x , pins_bitmask: %x hasn't been found.\n",
                    reinterpret_cast<unsigned int>(peripheral),
                    static_cast<unsigned int>(pins_bitmask.to_ulong()));
            return;
        } else {
             interrupt_list.erase(list_iter);
        }
         */
    }

    std::list<GPIO::GPIOPins>::iterator EXTIInt::find_gpiopins(std::list<GPIO::GPIOPins> &gpiopins_list,
                                                        GPIO_TypeDef *peripheral, std::bitset<16> pins_bitmask)
    {
        auto return_list_iter = gpiopins_list.end();
        for (auto list_iter = gpiopins_list.begin();list_iter != gpiopins_list.end(); list_iter++)
        {
            if ( (*list_iter).get_peripheral() == peripheral
               && (*list_iter).get_pins_bitmask() == pins_bitmask) {
                return_list_iter = list_iter;
                break;
            }

        }
        return return_list_iter;
    }

    IRQn_Type EXTIInt::get_irqn_type(GPIO::Pin pin)
    {
        switch(pin)
        {
            case GPIO::Pin::P0: return EXTI0_IRQn;
            case GPIO::Pin::P1: return EXTI1_IRQn;
            case GPIO::Pin::P2: return EXTI2_IRQn;
            case GPIO::Pin::P3: return EXTI3_IRQn;
            case GPIO::Pin::P4: return EXTI4_IRQn;

            case GPIO::Pin::P5:
            case GPIO::Pin::P6:
            case GPIO::Pin::P7:
            case GPIO::Pin::P8:
            case GPIO::Pin::P9: return EXTI9_5_IRQn;

            case GPIO::Pin::P10:
            case GPIO::Pin::P11:
            case GPIO::Pin::P12:
            case GPIO::Pin::P13:
            case GPIO::Pin::P14:
            case GPIO::Pin::P15: return EXTI15_10_IRQn;
        }

        return (IRQn_Type)-100;
    }

    std::list<GPIO::GPIOPins> EXTIInt::interrupt_list;
}
