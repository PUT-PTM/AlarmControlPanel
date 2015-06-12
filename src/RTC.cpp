#include "RTC.hpp"

void DateTime::initialize()
{
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

void DateTime::configure_calendar()
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /* Configure the Date */
    /* Set Date: Friday March 13th 2015 */
    sdatestructure.Year = 0x15;
    sdatestructure.Month = RTC_MONTH_MARCH;
    sdatestructure.Date = 0x13;
    sdatestructure.WeekDay = RTC_WEEKDAY_FRIDAY;

    if(HAL_RTC_SetDate(&handle, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK) {
        /* Initialization Error */
        while(true)
            ;
    }

    /* Configure the Time */
    /* Set Time: 02:00:00 */
    stimestructure.Hours = 0x02;
    stimestructure.Minutes = 0x00;
    stimestructure.Seconds = 0x00;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
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
