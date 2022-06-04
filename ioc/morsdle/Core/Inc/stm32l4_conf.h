//
// Created by Aidan Morgan on 4/6/2022.
//

#ifndef MORSDLE_STM32L4_CONF_H
#define MORSDLE_STM32L4_CONF_H

#include "main.h"

struct stm32l4_conf {
    SPI_HandleTypeDef* spi_handle;
    TIM_HandleTypeDef* timer_handle;
    UART_HandleTypeDef* uart_handle;

    GPIO_Port dc_port;
    GPIO_Pin dc_pin;

    GPIO_Port rst_port;
    GPIO_Pin rst_pin;

    GPIO_Port busy_port;
    GPIO_Pin busy_pin;

    GPIO_Port cs_port;
    GPIO_Pin cs_pin;

    GPIO_Port button_port;
    GPIO_Pin button_pin;

    GPIO_Port led_port;
    GPIO_Pin led_pin;
};

typedef struct stm32l4_conf stm32l4_conf_t;

#endif //MORSDLE_STM32L4_CONF_H
