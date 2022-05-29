//
// Created by Aidan Morgan on 27/5/2022.
//

#ifndef MORSDLE_WAVESHARE_STM_H
#define MORSDLE_WAVESHARE_STM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"

enum wavesharestm_pin {
    RST,
    DC,
    CS,
    BUSY
};

typedef enum wavesharestm_pin wavesharestm_pin_t;

struct wavesharestm_pin_conf {
    uint8_t port;
    uint8_t pin;
};

struct wavesharestm_conf {
    struct wavesharestm_pin_conf rst;
    struct wavesharestm_pin_conf dc;
    struct wavesharestm_pin_conf cs;
    struct wavesharestm_pin_conf busy;

    uint16_t width;
    uint16_t height;
};

typedef struct wavesharestm_conf * wavesharestm_conf_t;

static void wavesharestm_digital_write(wavesharestm_conf_t conf, wavesharestm_pin_t pin, uint8_t val);
static void wavesharestm_digital_read(wavesharestm_conf_t conf, wavesharestm_pin_t pin, uint8_t* result);
static void wavesharestm_spi_write(wavesharestm_conf_t conf, uint8_t data);
static void wavesharestm_spi_read(wavesharestm_conf_t conf, uint8_t* data);
static void wavesharestm_delay(wavesharestm_conf_t conf, uint32_t ms);
static void wavesharestm_waitbusy_high(wavesharestm_conf_t conf);
static void wavesharestm_waitbusy_low(wavesharestm_conf_t conf);

// public interface methods that are used by the waveshare driver, but are specialised for writing to a STM32 device
void wavesharestm_init(wavesharestm_conf_t conf);
void wavesharestm_destroy(wavesharestm_conf_t conf);
void wavesharestm_send_data(wavesharestm_conf_t conf, uint8_t data);
void wavesharestm_send_command(wavesharestm_conf_t conf, uint8_t data);
void wavesharestm_clear(wavesharestm_conf_t conf, uint8_t colour);
void wavesharestm_reset(wavesharestm_conf_t conf);

void wavesharestm_render_impl(void * data, uint8_t* buffer, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

#endif //MORSDLE_WAVESHARE_STM_H
