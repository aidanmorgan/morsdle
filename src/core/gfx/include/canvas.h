//
// Created by aidan on 21/04/2022.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "cbuff.h"
#include "imagebuffer.h"

typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;


// an opaque handle, defined by the implementor
typedef struct display_impl_t display_impl_t;

// defines a region of the display that has been dirtied and needs to be redrawn
typedef struct {
    point_t top_left;
    point_t bottom_right;
} rectangle_t;

typedef enum {
    ROTATION_NONE = 0,
    ROTATION_NINETY = 90,
    ROTATION_ONEHINDREDEIGHTY = 180,
    ROTATION_TWOHUNDREDSEVENTY = 270
} rotation_t;

// forward declaration
typedef struct canvas canvas_t;


// collects up all regions that need to be redrawn for the provided handle
typedef struct {
    // a circular buffer of region_t
    cbuff_t *dirty_regions;
    canvas_t *canvas;
    display_impl_t *display;
} render_pass_t;


struct canvas {
    void (*draw_line)(render_pass_t *pass, display_impl_t *display, point_t start, point_t end, uint8_t thickness,
                      colour_t colour);

    void (*fill_rect)(render_pass_t *pass, display_impl_t *display, point_t topleft, point_t bottomright,
                      colour_t fill_colour);

    void (*draw_char)(render_pass_t *pass, display_impl_t *display, char c, point_t topleft, uint8_t size, colour_t colour);

    void (*clear)(render_pass_t *pass, display_impl_t *display, colour_t colour);

    uint16_t height;
    uint16_t width;
    rotation_t rotation;
};


extern void canvas_init(canvas_t *ops, rotation_t rotation);

extern void canvas_destroy(canvas_t *ops);

size_t renderpass_regions_size(render_pass_t* pass);
bool renderpass_clear_regions(render_pass_t* pass);

size_t renderpass_read_regions(render_pass_t* pass, rectangle_t* out, size_t size);
bool renderpass_add_region(render_pass_t* pass, point_t ul, point_t br);

// start a rendering pass, indicating to the underlyign display that we are going to soon be
// sending updated dirty regions
extern void render_pass_init(render_pass_t *render);

// end the render pass, update the display with the buffer in the handle, updating the regions
// that are marked
extern void render_pass_end(render_pass_t *render);



#endif //DISPLAY_H
