//
// Created by Aidan Morgan on 20/6/2022.
//
#include "waveshare_spi.h"
#include "main.h"



// currently configured to PC7
extern void wavesharespi_write_dc(uint8_t val) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, val == 0 ?  GPIO_PIN_RESET : GPIO_PIN_SET);
}


// currently configured to PC9
extern void wavesharespi_write_cs(uint8_t val) {
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, val == 0 ?  GPIO_PIN_RESET : GPIO_PIN_SET);
}

// currently configured to PC8
extern void wavesharespi_write_rst(uint8_t val) {
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, val == 0 ?  GPIO_PIN_RESET : GPIO_PIN_SET);
}

extern SPI_HandleTypeDef hspi2;

// currently configured to SPI2
extern void wavesharespi_write_spi(uint8_t val) {
    HAL_SPI_Transmit(&hspi2, &val, 1, 1000);
}

// currently configured to PC6
extern uint8_t wavesharespi_read_busy() {
    return HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin);
}

extern void wavesharespi_delay(uint32_t time) {
    HAL_Delay(time);
}

extern uint32_t wavesharespi_ticks() {
    return HAL_GetTick();
}