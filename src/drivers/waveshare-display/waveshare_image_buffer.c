//
// Created by Aidan Morgan on 7/6/2022.
//
#include "waveshare_image_buffer.h"

waveshare_ib_error_t wavehsare_ib_init(waveshare_image_buffer_t* buffer) {
//    buffer->buffer = (uint8_t*)malloc(sizeof(uint8_t) * (buffer->width / 2) * (buffer->height / 2));
    return WAVESHARE_IB_OK;
}


void waveshare_ib_set_pixel(waveshare_image_buffer_t* buffer, uint16_t x, uint16_t y, waveshare_image_colour_t colour) {
    uint16_t x_idx = x / 2;
    uint16_t y_idx = y /2;

    uint8_t* data = &(buffer->buffer[(y_idx * (buffer->width / 2) + x_idx)]);

    x = x %2;
    y = y % 2;

    if(x == 0 && y == 0) {
        (*data) = SET_VAL((*data), 2, 0, colour);
    }
    else if(x == 1 && y == 0) {
        (*data) =SET_VAL((*data), 2, 2, colour);
    }
    else if(x== 0 && y == 1) {
        (*data) =SET_VAL((*data), 2, 4, colour);
    }
    else if(x==1 && y == 1) {
        (*data) =SET_VAL((*data), 2, 6, colour);
    }
}

void waveshare_ib_get_pixel(waveshare_image_buffer_t*buffer, uint16_t x, uint16_t y, waveshare_image_colour_t* colour) {
    uint16_t x_idx = x / 2;
    uint16_t y_idx = y /2;

    uint8_t* data = &(buffer->buffer[(y_idx * (buffer->width / 2) + x_idx)]);

    x = x %2;
    y = y % 2;

    if(x == 0 && y == 0) {
        (*colour) = (waveshare_image_colour_t)GET_VAL((*data), 2, 0);
    }
    else if(x == 1 && y == 0) {
        (*colour) = (waveshare_image_colour_t)GET_VAL((*data), 2, 2);
    }
    else if(x== 0 && y == 1) {
        (*colour) = (waveshare_image_colour_t)GET_VAL((*data), 2, 4);
    }
    else if(x==1 && y == 1) {
        (*colour) = (waveshare_image_colour_t)GET_VAL((*data), 2, 6);
    }

}
