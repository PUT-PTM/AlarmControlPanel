#include "RTC.hpp"

void DateTime::initialize(void *ignored) {
    debug("RTC: Initializing RTC\n");

    handle.Instance = RTC;
    handle.Init.HourFormat = RTC_HOURFORMAT_24;
    handle.Init.AsynchPrediv = 0x7F;
    handle.Init.SynchPrediv = 0x00FF;
    handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&handle) != HAL_OK) {
        /* Initialization Error */
        debug("RTC: RTC initialization error\n");
        while(true)
            ;
    }

    /* Read the Back Up Register0 Data */
    if(HAL_RTCEx_BKUPRead(&handle, RTC_BKP_DR0) != 0x32F2) {
        /* Configure RTC Calendar */
        configure_calendar();
    }

    debug("RTC: INITIALIZED\n");

    get_date_and_time();

    vTaskDelete(NULL);
}

void DateTime::send_request() {
    debug("RTC: Sending request\n");

    std::string request = "GET /?tz=Europe/Warsaw HTTP/1.1\r\n"
                          "Host: time.dpieczynski.pl\r\n"
                          "Connection: close"
                          "Accept: application/json\r\n"
                          "\r\n\r\n";

    if (FreeRTOS_send(current_socket, request.c_str(), request.length(), 0) != request.length()) {
        debug("RTC: Error sending request.\n");
    }
}

std::string DateTime::receive_response() {
    debug("RTC: Receiving response\n");

    std::string response;
    response.reserve(350);

    char chr;
    while (FreeRTOS_recv(current_socket, &chr, 1, 0) > 0) {
        response += chr;
    }

    debug("RTC: Response received\n");

    size_t end_of_header = response.find("\r\n\r\n") + 4;
    return response.substr(end_of_header);
}

uint8_t DateTime::convert_from_hex(int number) {
    std::stringstream ss;
    ss << number;

    std::string hexadecimal = ss.str();
    uint8_t converted = 0;

    uint8_t current_power = hexadecimal.length() - 1;
    for(const auto &chr : hexadecimal) {
        converted += (chr - '0') * std::pow(16, current_power);
        current_power--;
    }

    return converted;
}

JSON DateTime::download_time() {
    uint32_t host = FreeRTOS_gethostbyname("time.dpieczynski.pl");
    if ((host == 0)) {
        debug("RTC: Error retrieving DNS information.\n");
        return JSON();
    }

    debug("RTC: Creating FreeRTOS socket\n");
    current_socket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM,
                                     FREERTOS_IPPROTO_TCP);

    if (current_socket == FREERTOS_INVALID_SOCKET) {
        debug("RTC: Error creating socket.\n");
        return JSON();
    }

    struct freertos_sockaddr client;
    client.sin_port = FreeRTOS_htons(80);
    client.sin_addr = host;

    if (FreeRTOS_connect(current_socket, &client, sizeof(client)) != 0) {
        FreeRTOS_closesocket(current_socket);
        debug("RTC: Socket connect failed.\n");
        return JSON();
    }

    send_request();

    return JSON(receive_response());
}

void DateTime::configure_calendar() {
    debug("RTC: Configuring calendar\n");

    JSON json = download_time();
    if (json.is_empty()) {
        // TODO: do something when time cannot be downloaded
        return;
    }

    debug("RTC: Time downloaded\n");

    /* Configure the Date */
    RTC_DateTypeDef date;
    date.Year = json.get<int>("year") - 2000;
    date.Month = json.get<int>("month");
    date.Date = convert_from_hex(json.get<int>("day"));
    date.WeekDay = json.get<int>("weekday");

    if(HAL_RTC_SetDate(&handle, &date, RTC_FORMAT_BCD) != HAL_OK) {
        /* Initialization Error */
        while(true)
            ;
    }

    /* Configure the Time */
    RTC_TimeTypeDef time;
    time.Hours = convert_from_hex(json.get<int>("hour"));
    time.Minutes = convert_from_hex(json.get<int>("minute"));
    time.Seconds = convert_from_hex(json.get<int>("second"));
    time.TimeFormat = RTC_HOURFORMAT_24;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&handle, &time, RTC_FORMAT_BCD) != HAL_OK) {
        /* Initialization Error */
        while(true)
            ;
    }

    /* Writes a data in a RTC Backup data Register0 */
    HAL_RTCEx_BKUPWrite(&handle, RTC_BKP_DR0, 0x32F2);

    /* Shutdown and close socket */
    FreeRTOS_shutdown(current_socket, FREERTOS_SHUT_RDWR);
    FreeRTOS_closesocket(current_socket);
}

std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t> DateTime::get_date_and_time()
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    HAL_RTC_GetTime(&handle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&handle, &date, RTC_FORMAT_BIN);

    debug("RTC: TIME: %u:%u:%u\n", time.Hours, time.Minutes, time.Seconds);
    debug("RTC: DATE: %u.%u.%u\n", date.Date, date.Month, 2000 + date.Year);

    return std::make_tuple(date.Date, date.Month, 2000 + date.Year, time.Hours, time.Minutes, time.Seconds);
}

RTC_HandleTypeDef DateTime::handle;
xSocket_t DateTime::current_socket;
