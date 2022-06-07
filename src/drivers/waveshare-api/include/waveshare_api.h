//
// Created by Aidan Morgan on 27/5/2022.
//

#ifndef MORSDLE_WAVESHARE_API_H
#define MORSDLE_WAVESHARE_API_H

#include <stdint.h>
#include <stdbool.h>
#include "imagebuffer.h"

#define WAVESHARE_BLACK   0x0	/// 000
#define WAVESHARE_WHITE   0x1	///	001
#define WAVESHARE_GREEN   0x2	///	010
#define WAVESHARE_BLUE    0x3	///	011
#define WAVESHARE_RED     0x4	///	100
#define WAVESHARE_YELLOW  0x5	///	101
#define WAVESHARE_ORANGE  0x6	///	110

typedef struct {
    uint8_t * buffer;
    uint16_t width;
    uint16_t height;
} image_buffer_t;

const uint8_t buffer_to_waveshare_lookup[4] = {
        WAVESHARE_WHITE,  // clear
        WAVESHARE_ORANGE,// orange
        WAVESHARE_GREEN, // green
        WAVESHARE_BLACK  // black
};

// public interface methods that are used by the waveshare driver, but are specialised for writing to a STM32 device
void waveshareapi_init(imagebuffer_t *);
void waveshareapi_destroy(imagebuffer_t *);
void waveshareapi_send_data(uint8_t data);
void waveshareapi_send_command(uint8_t data);
void waveshareapi_reset();
void waveshareapi_wake();
void waveshareapi_sleep();

void waveshareapi_render_region(imagebuffer_t* buffer, uint16_t width, uint16_t height, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

#endif //MORSDLE_WAVESHARE_API_H
