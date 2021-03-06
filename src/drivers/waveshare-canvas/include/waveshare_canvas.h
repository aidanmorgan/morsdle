//
// Created by aidan on 21/04/2022.
//

#ifndef WAVESHARE_DISPLAY_H
#define WAVESHARE_DISPLAY_H

#include "morsdle.h"
#include "canvas.h"
#include "renderer.h"
#include "imagebuffer.h"
#include <assert.h>


#define WAVESHARE_PIXEL_WIDTH 600
#define WAVESHARE_PIXEL_HEIGHT 448

typedef struct  {
    const uint8_t *table;
    uint16_t width;
    uint16_t height;
} font_t;

extern font_t font_table;

struct display_impl_t {
    imagebuffer_t *buffer;

    // this is the callback to perform the actual render, required to be set by the implementation
    void (*render_region)(imagebuffer_t *, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);

    void (*init)();

    // callback to wake the display up if it's been put to sleep
    void (*pre_render)();

    // callback to put the display to sleep if it's been sleeping
    void (*post_render)();
};

void waveshare_draw_line(render_pass_t *pass, display_impl_t *display, point_t start, point_t end, uint8_t thickness,
                         colour_t colour);

void waveshare_fill_rect(render_pass_t *pass, display_impl_t *display, point_t topleft, point_t bottomright,
                         colour_t fill_colour);

void waveshare_draw_char(render_pass_t *pass, display_impl_t *display, char c, point_t topleft, uint8_t size,
                         colour_t colour);

void waveshare_clear(render_pass_t *pass, display_impl_t *display, colour_t colour);

#endif //WAVESHARE_DISPLAY_H
