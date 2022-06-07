//
// Created by Aidan Morgan on 7/6/2022.
//

#ifndef MORSDLE_IMAGEBUFFER_H
#define MORSDLE_IMAGEBUFFER_H
#include <stdint.h>
#include <stdlib.h>

// length, offset
#define MASK(L,P)            (~(0xff << (L)) << (P))
// value, length, offset
#define GET_VAL(BF,L,P)      (((BF) & MASK(L,P)) >> P)
// value, length, offset, newvalue
#define SET_VAL(BF,L,P,V)    ( (BF) = ((BF) & ~MASK(L,P)) | (((V) << (P)) & MASK(L,P)) )


#define WAVESHARE_IB_BUFFER_SIZE(l,w) ((l / 2) * (w / 2) * sizeof(uint8_t))

typedef enum {
    IMAGEBUFFER_OK,
    IMAGEBUFFER_INIT_FAILED
}imagebuffer_error_t;

typedef enum {
    IMAGEBUFFER_CLEAR =0,
    IMAGEBUFFER_ORANGE = 1,
    IMAGEBUFFER_GREEN = 2,
    IMAGEBUFFER_BLACK = 3,
    IMAGEBUFFER_INVALID = 255
} imagebuffer_colour_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t* buffer;
    uint32_t buffer_size;
} imagebuffer_t;

imagebuffer_error_t imagebuffer_init(imagebuffer_t* buffer, uint16_t width, uint16_t height);

void imagebuffer_setpixel(imagebuffer_t* buffer, uint16_t x, uint16_t y, imagebuffer_colour_t colour);
void imagebuffer_getpixel(imagebuffer_t*buffer, uint16_t x, uint16_t y, imagebuffer_colour_t* colour);

#endif //MORSDLE_IMAGEBUFFER_H
