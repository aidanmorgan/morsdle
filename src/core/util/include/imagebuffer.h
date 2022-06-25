//
// Created by Aidan Morgan on 7/6/2022.
//

#ifndef MORSDLE_IMAGEBUFFER_H
#define MORSDLE_IMAGEBUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>


// length, offset
#define MASK(L, P)            (~(0xffU << (L)) << (P))
// value to read from, length (in bits), offset (in bits)
#define GET_VAL(BF, L, P)      (((BF) & MASK(L,P)) >> P)
// value to update, length (in bits), offset (in bits), newvalue (value to set)
#define SET_VAL(BF, L, P, V)    ( (BF) = ((BF) & ~MASK(L,P)) | (((V) << (P)) & MASK(L,P)) )

typedef enum {
    IMAGEBUFFER_OK,
    IMAGEBUFFER_INIT_FAILED
} imagebuffer_error_t;

typedef enum {
    COLOUR_WHITE = 0,
    COLOUR_ORANGE = 1,
    COLOUR_GREEN = 2,
    COLOUR_BLACK = 3,
    COLOUR_INVALID = 255
} colour_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t buffer_size;
    uint8_t *buffer;
} imagebuffer_t;


/*
 *  Okay, here comes a bit of an essay. Because I am attempting to keep the waveshare board code out of my
 *  code, I need to be able to perform render operations against an image buffer that is then sent to
 *  the display (whatever that is). The STM32L476RG board I am using has limited heap space, so I can't
 *  allocate a giant image buffer of uint8_t * length * width pixels as it just hard-faults the board.
 *
 *  The best packing I can perform here is to use the fact that we only use 4 colours, and then encode
 *  pixels into two bits each.
 *
 *  Just to be confusing, the waveshare board encodes pixels as 4 bits, so two pixels per row.
 *
 *  To try and be clever, I am actually encoding the x and y values, so I am encoding a 2x2 matrix of
 *  pixels into each byte, that means that each "row" in the data is actually 2-pixels high and 2-pixels
 *  wide.
 *
 *  The order is x0, x1, y0, y1 for reference.
 */
imagebuffer_error_t imagebuffer_init(imagebuffer_t *buffer, uint16_t width, uint16_t height);

void imagebuffer_setpixel(imagebuffer_t *buffer, uint16_t x, uint16_t y, colour_t colour);

void imagebuffer_getpixel(imagebuffer_t *buffer, uint16_t x, uint16_t y, colour_t *colour);

#endif //MORSDLE_IMAGEBUFFER_H
