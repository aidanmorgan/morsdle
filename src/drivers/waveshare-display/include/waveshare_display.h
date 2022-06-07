//
// Created by aidan on 21/04/2022.
//

#ifndef WAVESHARE_DISPLAY_H
#define WAVESHARE_DISPLAY_H


#include "morsdle.h"
#include "display.h"
#include "renderer.h"
#include <assert.h>

#define WAVESHARE_BLACK   0x0	/// 000
#define WAVESHARE_WHITE   0x1	///	001
#define WAVESHARE_GREEN   0x2	///	010
#define WAVESHARE_BLUE    0x3	///	011
#define WAVESHARE_RED     0x4	///	100
#define WAVESHARE_YELLOW  0x5	///	101
#define WAVESHARE_ORANGE  0x6	///	110

#define WAVESHARE_PIXEL_WIDTH 600
#define WAVESHARE_PIXEL_HEIGHT 448
#define WAVESHARE_PIXELS_PER_BYTE_WIDE 2
#define WAVESHARE_PIXELS_PER_BYTE_HIGH 2

#define WAVESHARE_BUFFER_SIZE ((WAVESHARE_PIXEL_WIDTH/WAVESHARE_PIXELS_PER_BYTE_WIDE)*(WAVESHARE_PIXEL_HEIGHT/WAVESHARE_PIXELS_PER_BYTE_HIGH))

struct display_impl_t  {
    uint8_t* buffer;
    uint32_t buffersize;
    // this is the callback to perform the actual render, required to be set by the implementation
    void (*render_dirty_region)(uint8_t*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);

    void (*init)();

    // callback to wake the display up if it's been put to sleep
    void (*pre_render)();

    // callback to put the display to sleep if it's been sleeping
    void (*post_render)();
};

void waveshare_draw_line(render_pass_t* pass, point_t start, point_t end, uint8_t thickness, colour_t colour);
void waveshare_fill_rect(render_pass_t* pass,point_t topleft, point_t bottomright, colour_t fill_colour);
void waveshare_draw_char(render_pass_t* pass, char c, point_t topleft, uint8_t size, colour_t colour);
void waveshare_clear(render_pass_t* pass, colour_t colour);

#endif //WAVESHARE_DISPLAY_H
