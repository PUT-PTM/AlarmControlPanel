#ifndef RTC_HPP
#define RTC_HPP

#include <iostream>
#include <string>

#include "debug.h"
#include "stm32f4xx_hal.h"

class DateTime {
    private:
        static RTC_HandleTypeDef handle;
    public:
        static void initialize();
        static void configure_calendar();
        static void get_date_and_time();
};

#endif // RTC_HPP
