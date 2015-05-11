#include "Random.hpp"

Random::Random() {
    if (not _number_of_instances) {
        _handle = RNG_HandleTypeDef();
        _handle.Instance = RNG;

        __HAL_RCC_RNG_CLK_ENABLE();
        HAL_RNG_Init(&_handle);
        while(_handle.State != HAL_RNG_STATE_READY);
    }

    _number_of_instances++;
}

Random::~Random() {
    _number_of_instances--;

    if (not _number_of_instances) {
        HAL_RNG_DeInit(&_handle);
        __HAL_RCC_RNG_CLK_DISABLE();
    }    
}

uint32_t Random::random() {
    uint32_t *random_number = nullptr;
    HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&_handle, random_number);
    while(status != HAL_OK);

    return _handle.RandomNumber;
}

uint32_t Random::randrange(uint32_t start, uint32_t stop) {
    uint32_t *random_number = nullptr;
    HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&_handle, random_number);
    while(status != HAL_OK);

    return start + _handle.RandomNumber % (stop - start);
}

uint8_t Random::_number_of_instances = 0;
RNG_HandleTypeDef Random::_handle;
