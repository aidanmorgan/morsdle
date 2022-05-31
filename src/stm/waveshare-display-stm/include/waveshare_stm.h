//
// Created by Aidan Morgan on 27/5/2022.
//

#ifndef MORSDLE_WAVESHARE_STM_H
#define MORSDLE_WAVESHARE_STM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"


struct wavesharestm_pin {
    GPIO_TypeDef* port;
    uint16_t pin;
};
typedef struct wavesharestm_pin wavesharestm_pin_t;

extern wavesharestm_pin_t WAVESHARESTM_RST_PIN;

// data/command control pin, write command when DC=0; write data when DC=1
extern wavesharestm_pin_t WAVESHARESTM_DC_PIN;

// Slave chip select, when CS is low, the chip is enabled
extern wavesharestm_pin_t WAVESHARESTM_CS_PIN;
extern wavesharestm_pin_t WAVESHARESTM_BUSY_PIN;
extern SPI_HandleTypeDef WAVESHARE_SPI_HANDLE;
extern UART_HandleTypeDef WAVESHARE_UART_HANDLE;


// public interface methods that are used by the waveshare driver, but are specialised for writing to a STM32 device
void wavesharestm_init();
void wavesharestm_destroy();
void wavesharestm_send_data(uint8_t data);
void wavesharestm_send_command(uint8_t data);
void wavesharestm_reset();

void wavesharestm_wake();
void wavesharestm_sleep();

void wavesharestm_render_impl(uint8_t* buffer, uint16_t width, uint16_t height, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

#endif //MORSDLE_WAVESHARE_STM_H
