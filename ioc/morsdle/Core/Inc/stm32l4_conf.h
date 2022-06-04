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

    GPIO_TypeDef * dc_port;
    uint16_t dc_pin;

    GPIO_TypeDef * rst_port;
    uint16_t rst_pin;

    GPIO_TypeDef * busy_port;
    uint16_t busy_pin;

    GPIO_TypeDef * cs_port;
    uint16_t cs_pin;

    GPIO_TypeDef * button_port;
    uint16_t button_pin;

    GPIO_TypeDef * led_port;
    uint16_t led_pin;
};

typedef struct stm32l4_conf stm32_config_t;

#endif //MORSDLE_STM32L4_CONF_H
