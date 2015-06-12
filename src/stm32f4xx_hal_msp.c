#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
}

void HAL_MspDeInit(void)
{
}

void HAL_PPP_MspInit(void)
{
}

void HAL_PPP_MspDeInit(void)
{
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /* Configue LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        while(1)
            ;
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        while(1)
            ;
    }
    
    /* Enable RTC Clock */
    __HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
    __HAL_RCC_RTC_DISABLE();
}
