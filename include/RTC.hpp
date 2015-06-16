#ifndef RTC_HPP
#define RTC_HPP

#include <iostream>
#include <string>
#include <sstream>

#include "debug.h"
#include "document.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

class DateTime {
    private:
        static RTC_HandleTypeDef handle;
    public:
        static void initialize(void *ignored);
        static void send_request(xSocket_t socket);
        static std::string receive_response(xSocket_t socket);
        static void configure_calendar();
        static void get_date_and_time();
};

#endif // RTC_HPP
