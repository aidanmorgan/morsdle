//
// Created by aidan on 21/04/2022.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "cbuff.h"

typedef struct {
    uint16_t x;
    uint16_t y;
}point;
typedef point point_t;

typedef enum {
    COLOUR_BLACK,
    COLOUR_WHITE,
    COLOUR_GREEN,
    COLOUR_BLUE,
    COLOUR_RED,
    COLOUR_YELLOW,
    COLOUR_ORANGE
} colour_t;

// an opaque handle, defined by the implementor
typedef struct display_handle_t display_handle_t;

// defines a region of the display that has been dirtied and needs to be redrawn
struct region {
    point_t top_left;
    point_t bottom_right;
};
typedef struct region region_t;

// collects up all regions that need to be redrawn for the provided handle
struct render_pass {
    cbuff_t regions;
    display_handle_t* handle;
};
typedef struct render_pass* render_pass_t;

typedef struct {
    void (*draw_line)(render_pass_t pass, point_t start, point_t end, uint8_t thickness, colour_t colour);
    void (*fill_rect)(render_pass_t pass, point_t topleft, point_t bottomright, colour_t fill_colour);
    void (*draw_char)(render_pass_t pass, char c, point_t topleft, uint8_t size, colour_t colour);

    display_handle_t* handle;

    uint16_t height;
    uint16_t width;
    cbuff_t dirty_regions;
} display;
typedef display* display_t;

extern void display_init(display_t ops);
extern void display_destroy(display_t ops);

// start a rendering pass, indicating to the underlyign display that we are going to soon be
// sending updated dirty regions
extern void start_render_pass(display_handle_t* handle, render_pass_t render);

// end the render pass, update the display with the buffer in the handle, updating the regions
// that are marked
extern void end_render_pass(render_pass_t render);

#endif //DISPLAY_H
