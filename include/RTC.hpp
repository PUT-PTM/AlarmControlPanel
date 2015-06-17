#ifndef RTC_HPP
#define RTC_HPP

#include <string>
#include <sstream>
#include <cmath>

#include "debug.h"
#include "JSON.hpp"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

class DateTime {
    private:
        static RTC_HandleTypeDef handle;
        static xSocket_t current_socket;
        static uint8_t convert_from_hex(int number);
        static void send_request();
        static std::string receive_response();
        static JSON download_time();
        static bool initialized;
    public:
        static void initialize(void *ignored);
        static bool is_initialized() {return initialized;};
        static void configure_calendar();
        static std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t> get_date_and_time();
        static std::string get_date_and_time_as_string();
};

#endif // RTC_HPP
