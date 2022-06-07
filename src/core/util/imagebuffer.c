//
// Created by Aidan Morgan on 7/6/2022.
//
#include "imagebuffer.h"

imagebuffer_error_t imagebuffer_init(imagebuffer_t *buffer, uint16_t width, uint16_t height) {
    buffer->width = width;
    buffer->height = height;
    buffer->buffer_size = (buffer->width / 2) * (buffer->height / 2);

    uint8_t *tmp = (uint8_t *) malloc(sizeof(uint8_t) * buffer->buffer_size);
    if (tmp == NULL) {
        return IMAGEBUFFER_INIT_FAILED;
    }

    // initialise with white pixels throughout the image
    memset(tmp, 0, buffer->buffer_size);
    buffer->buffer = tmp;

    return IMAGEBUFFER_OK;
}


void imagebuffer_setpixel(imagebuffer_t *buffer, uint16_t x, uint16_t y, imagebuffer_colour_t colour) {
    if (colour == IMAGEBUFFER_INVALID) {
        return;
    }

    uint16_t x_idx = x / 2;
    uint16_t y_idx = y / 2;

    uint8_t *data = &(buffer->buffer[(y_idx * (buffer->width / 2) + x_idx)]);

    x = x % 2;
    y = y % 2;

    // x,y = idx
    // 0, 0, = 0
    // 1, 0 = 2
    // 0, 1 = 4
    // 1, 1 = 6
    (*data) = SET_VAL((*data), 2, ((2*x)+(4*y)), colour);
}

void imagebuffer_getpixel(imagebuffer_t *buffer, uint16_t x, uint16_t y, imagebuffer_colour_t *colour) {
    uint16_t x_idx = x / 2;
    uint16_t y_idx = y / 2;

    uint8_t *data = &(buffer->buffer[(y_idx * (buffer->width / 2) + x_idx)]);

    x = x % 2;
    y = y % 2;

    // x,y = idx
    // 0, 0, = 0
    // 1, 0 = 2
    // 0, 1 = 4
    // 1, 1 = 6
    (*colour) = (imagebuffer_colour_t) GET_VAL((*data), 2, ((2*x)+(4*y)));
}
