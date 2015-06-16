#include "RTC.hpp"

void DateTime::initialize(void *ignored) {
    debug("Initializing RTC\n");

    handle.Instance = RTC;
    handle.Init.HourFormat = RTC_HOURFORMAT_24;
    handle.Init.AsynchPrediv = 0x7F;
    handle.Init.SynchPrediv = 0x00FF;
    handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&handle) != HAL_OK) {
        /* Initialization Error */
        debug("RTC initialization error\n");
        while(true)
            ;
    }

    /* Read the Back Up Register0 Data */
    if(HAL_RTCEx_BKUPRead(&handle, RTC_BKP_DR0) != 0x32F2) {
        /* Configure RTC Calendar */
        configure_calendar();
    }
}

void DateTime::send_request(xSocket_t socket) {
    std::stringstream ss;
    ss << "GET /" << "?tz=Europe/Warsaw HTTP/1.1\r\n"
    << "Host: time.dpieczynski.pl\r\n"
    << "Connection: close"
    << "Accept: application/json\r\n"
    << "\r\n\r\n";
    std::string request = ss.str();
    
    if (FreeRTOS_send(socket, request.c_str(), request.length(), 0) != request.length()) {
        debug("Error sending request.\n");
    }
}

std::string DateTime::receive_response(xSocket_t socket) {
    std::stringstream buffer;

    char chr;
    while (FreeRTOS_recv(socket, &chr, 1, 0) > 0) {
        buffer << chr;
    }

    FreeRTOS_shutdown(socket, FREERTOS_SHUT_RDWR);
    FreeRTOS_closesocket(socket);

    std::string response = buffer.str();
    size_t end_of_header = response.find("\r\n\r\n") + 4;

    return response.substr(end_of_header);
}

void DateTime::configure_calendar() {
    debug("Configuring calendar\n");

    uint32_t host = FreeRTOS_gethostbyname("time.dpieczynski.pl");
    if ((host == 0)) {
        debug("Error retrieving DNS information.\n");
        return;
    }

    debug("Creating FreeRTOS socket\n");
    xSocket_t socket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM,
                                       FREERTOS_IPPROTO_TCP);

    if (socket == FREERTOS_INVALID_SOCKET) {
        debug("Error creating socket.\n");
        return;
    }

    debug("Binding to port\n");
    uint16_t port = 80;
    struct freertos_sockaddr client;
    client.sin_port = FreeRTOS_htons(port);
    client.sin_addr = host;
    if (FreeRTOS_bind(socket, &client, sizeof(&client)) != 0)
    {
        debug("Socket binding failed.\n");
        return;
    }

    if (FreeRTOS_connect(socket, &client, sizeof(client)) != 0) {        
        FreeRTOS_closesocket(socket);
        debug("Socket connect failed.\n");
        return;
    }

    send_request(socket);

    rapidjson::Document document;
    document.Parse(receive_response(socket).c_str());
                           
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /* Configure the Date */
    sdatestructure.Year = document["year"].GetInt() - 2000;
    sdatestructure.Month = document["month"].GetInt();
    sdatestructure.Date = document["day"].GetInt();
    sdatestructure.WeekDay = document["weekday"].GetInt();

    if(HAL_RTC_SetDate(&handle, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK) {
        /* Initialization Error */
        while(true)
            ;
    }

    /* Configure the Time */
    stimestructure.Hours = document["hour"].GetInt();
    stimestructure.Minutes = document["minute"].GetInt();
    stimestructure.Seconds = document["second"].GetInt();
    stimestructure.TimeFormat = RTC_HOURFORMAT_24;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&handle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK) {
        /* Initialization Error */
        while(true)
            ;
    }

    /* Writes a data in a RTC Backup data Register0 */
    HAL_RTCEx_BKUPWrite(&handle, RTC_BKP_DR0, 0x32F2);
}

void DateTime::get_date_and_time()
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    HAL_RTC_GetTime(&handle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&handle, &date, RTC_FORMAT_BIN);

    debug("%u:%u:%u\n", time.Hours, time.Minutes, time.Seconds);
    debug("%u.%u.%u\n", date.Date, date.Month, 2000 + date.Year);
}

RTC_HandleTypeDef DateTime::handle;
