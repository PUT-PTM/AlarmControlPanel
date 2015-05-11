#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "stm32f4xx_hal.h"

class Random {
    public:
        static uint8_t _number_of_instances;
        static RNG_HandleTypeDef _handle;

        Random();
        ~Random();
        /**
         * @brief Get random uint32_t
         */
        uint32_t random();
        /**
         * @brief Get random number from range <start; stop)
         * @param start min value (included in range)
         * @param stop  max value (excluded from range)
         */
        uint32_t randrange(uint32_t start, uint32_t stop);
};

#endif // RANDOM_HPP
