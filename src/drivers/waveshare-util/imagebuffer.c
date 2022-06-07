//
// Created by Aidan Morgan on 7/6/2022.
//
#include "imagebuffer.h"

imagebuffer_error_t imagebuffer_init(imagebuffer_t* buffer, uint16_t width, uint16_t height) {
    buffer->width = width;
    buffer->height = height;
    buffer->buffer_size = (buffer->width / 2) * (buffer->height / 2);

    uint8_t * tmp = (uint8_t*)malloc(sizeof(uint8_t) * buffer->buffer_size);
    buffer->buffer = tmp;
    return IMAGEBUFFER_OK;
}


void imagebuffer_setpixel(imagebuffer_t* buffer, uint16_t x, uint16_t y, imagebuffer_colour_t colour) {
    if(colour == IMAGEBUFFER_INVALID) {
        return;
    }

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

void imagebuffer_getpixel(imagebuffer_t*buffer, uint16_t x, uint16_t y, imagebuffer_colour_t* colour) {
    uint16_t x_idx = x / 2;
    uint16_t y_idx = y /2;

    uint8_t* data = &(buffer->buffer[(y_idx * (buffer->width / 2) + x_idx)]);

    x = x %2;
    y = y % 2;

    if(x == 0 && y == 0) {
        (*colour) = (imagebuffer_colour_t)GET_VAL((*data), 2, 0);
    }
    else if(x == 1 && y == 0) {
        (*colour) = (imagebuffer_colour_t)GET_VAL((*data), 2, 2);
    }
    else if(x== 0 && y == 1) {
        (*colour) = (imagebuffer_colour_t)GET_VAL((*data), 2, 4);
    }
    else if(x==1 && y == 1) {
        (*colour) = (imagebuffer_colour_t)GET_VAL((*data), 2, 6);
    }
}
