//
// Created by Aidan Morgan on 18/6/2022.
//

#ifndef MORSDLE_WAVESHARE_SPI_IMPL_H
#define MORSDLE_WAVESHARE_SPI_IMPL_H

#include "main.h"

extern SPI_HandleTypeDef hspi2;

#define WAVESHARESPI_WRITE_DC(x)     HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, (x) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define WAVESHARESPI_WRITE_CS(x)     HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, (x) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define WAVESHARESPI_WRITE_RST(x)    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, (x) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define WAVESHARESPI_WRITE_SPI(x)    HAL_SPI_Transmit(&hspi2, &(x), 1, 1000)
#define WAVESHARESPI_READ_BUSY()     HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin)
#define WAVESHARESPI_DELAY(x)        HAL_Delay((x))
#define WAVESHARESPI_TICKS()         HAL_GetTick()

#endif //MORSDLE_WAVESHARE_SPI_IMPL_H
