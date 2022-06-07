//
// Created by Aidan Morgan on 27/5/2022.
//

#ifndef MORSDLE_WAVESHARE_API_H
#define MORSDLE_WAVESHARE_API_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t * buffer;
    uint16_t width;
    uint16_t height;
} image_buffer_t;

// public interface methods that are used by the waveshare driver, but are specialised for writing to a STM32 device
void waveshareapi_init();
void waveshareapi_destroy();
void waveshareapi_send_data(uint8_t data);
void waveshareapi_send_command(uint8_t data);
void waveshareapi_reset();
void waveshareapi_wake();
void waveshareapi_sleep();

void waveshareapi_render_region(uint8_t*, uint16_t width, uint16_t height, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

#endif //MORSDLE_WAVESHARE_API_H
