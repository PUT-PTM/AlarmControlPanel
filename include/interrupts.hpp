#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP


#include "stm32f407xx.h"

namespace Interrupts {

     enum TIM {
          TIM2 = TIM2_IRQn,
          TIM3 = TIM3_IRQn,
          TIM4 = TIM4_IRQn
     };
}


#endif /* end of include guard: INTERRUPTS_HPP */

