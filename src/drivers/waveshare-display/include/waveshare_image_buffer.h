//
// Created by Aidan Morgan on 7/6/2022.
//

#ifndef MORSDLE_WAVESHARE_IMAGE_BUFFER_H
#define MORSDLE_WAVESHARE_IMAGE_BUFFER_H
#include <stdint.h>
#include <stdlib.h>

// length, offset
#define MASK(L,P)            (~(0xff << (L)) << (P))
// value, length, offset
#define GET_VAL(BF,L,P)      (((BF) & MASK(L,P)) >> P)
// value, length, offset, newvalue
#define SET_VAL(BF,L,P,V)    ( (BF) = ((BF) & ~MASK(L,P)) | (((V) << (P)) & MASK(L,P)) )


typedef enum {
    WAVESHARE_IB_OK,
    WAVESHARE_IB_INIT_FAILED
}waveshare_ib_error_t;

typedef enum {
    CLEAR =0,
    ORANGE = 1,
    GREEN = 2,
    BLACK = 3
} waveshare_image_colour_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t* buffer;
} waveshare_image_buffer_t;

waveshare_ib_error_t wavehsare_ib_init(waveshare_image_buffer_t* buffer);

void waveshare_ib_set_pixel(waveshare_image_buffer_t* buffer, uint16_t x, uint16_t y, waveshare_image_colour_t colour);
void waveshare_ib_get_pixel(waveshare_image_buffer_t*buffer, uint16_t x, uint16_t y, waveshare_image_colour_t* colour);

#endif //MORSDLE_WAVESHARE_IMAGE_BUFFER_H
