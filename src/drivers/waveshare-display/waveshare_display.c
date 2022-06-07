//
// Created by aidan on 21/04/2022.
//

#include <assert.h>
#include "waveshare_display.h"

#define MAX_DIRTY_REGIONS 64

// allocating the memory that backs the dirty_regions circular buffer
static rectangle_t dirty_regions_raw[MAX_DIRTY_REGIONS];
static cbuff_t dirty_regions = (cbuff_t) {};

static inline uint16_t max(uint16_t a, uint16_t b) {
    return a < b ? b : a;
}

static inline uint16_t min(uint16_t a, uint16_t b) {
    return a < b ? a : b;
}

struct font_t {
    char c;

    uint16_t length;
    uint16_t height;
    uint16_t width;
    uint32_t hex[];
};

typedef struct font_t font_t;

// FIXME : This is super gross and needs to be done a different way, but it will do for the moment
// FIXME : should probably just have these files on the file system, with some structure to allow different sizes etc
// FIXME : but this will do for the moment until I can be bothered incorporating TinyFS or something similar.
//
// So this contains a base64 encoded binary, of 1-bit per pixel, row-first order
//
const font_t font_buffer[1] = {
        (font_t) {
                .c = 'A',
                .length = 32,
                .height=64,
                .width=64,
                }
};


static colour_t colour_lookup[7] = {
        WAVESHARE_BLACK, //COLOUR_BLACK
        WAVESHARE_WHITE, //COLOUR_WHITE,
        WAVESHARE_GREEN, // COLOUR_GREEN,
        WAVESHARE_BLUE, // COLOUR_BLUE,
        WAVESHARE_RED, //COLOUR_RED,
        WAVESHARE_YELLOW, //COLOUR_YELLOW,
        WAVESHARE_ORANGE //COLOUR_ORANGE
};

static void waveshare_set_pixel(canvas_t* canvas, point_t point, colour_t colour) {
    if (point.x > canvas->width || point.y > canvas->height) {
        return;
    }

    uint64_t buffer_idx = (point.x / 2)  + (point.y * WAVESHARE_PIXELS_PER_BYTE_WIDE);
    if(buffer_idx > canvas->display_impl->buffersize) {
        assert(false);
    }

    uint8_t data = canvas->display_impl->buffer[buffer_idx];

    data = data & (~(0xF0 >> ((point.x % 2) * 4)));//Clear first, then set value
    data = data | ((colour_lookup[colour] << 4) >> ((point.x % 2) * 4));

    canvas->display_impl->buffer[buffer_idx] = data;
}

void waveshare_draw_line_impl(render_pass_t* pass, point_t start, point_t end, uint8_t thickness, colour_t colour, bool updateDirtyRegions) {
    uint16_t current_x = start.x;
    uint16_t current_y = start.y;

    int dx = (int) end.x - (int) start.x >= 0 ? end.x - start.x : start.x - end.x;
    int dy = (int) end.y - (int) start.y <= 0 ? end.y - start.y : start.y - end.y;

    int x_direction = start.x < end.x ? 1 : -1;
    int y_direction = start.y < end.y ? 1 : -1;

    //Cumulative error
    int cumulativeerror = dx + dy;

    for (;;) {
        waveshare_set_pixel(pass->canvas, (point_t) {.x = current_x, .y = current_y}, colour);

        if (2 * cumulativeerror >= dy) {
            if (current_x == end.x) {
                break;
            }

            cumulativeerror += dy;
            current_x += x_direction;
        }
        if (2 * cumulativeerror <= dx) {
            if (current_y == end.y)
                break;
            cumulativeerror += dx;
            current_y += y_direction;
        }
    }

    if(updateDirtyRegions) {
        cbuff_write(pass->dirty_regions, &(rectangle_t) {
                .top_left = start,
                .bottom_right = end
        });
    }
}

void waveshare_draw_line(render_pass_t* pass, point_t start, point_t end, uint8_t thickness, colour_t colour) {
    waveshare_draw_line_impl(pass, start, end, thickness, colour, true);
}


void waveshare_fill_rect(render_pass_t* pass, point_t topleft, point_t bottomright, colour_t fill_colour) {
    // moved outside the for loop to try and prevent the stack growing too big in the loop
    point_t point = (point_t) {};

    for(uint16_t x = topleft.x; x < topleft.x; x++) {
        for(uint16_t y = bottomright.y; y < bottomright.y; y++) {
            point.x = x;
            point.y = y;

            waveshare_set_pixel(pass->canvas, point, fill_colour);
        }
    }

    cbuff_write(pass->dirty_regions, &(rectangle_t) {
            .top_left = topleft,
            .bottom_right = bottomright
    });
}


void waveshare_draw_char(render_pass_t* pass, char c, point_t topleft, uint8_t size, colour_t colour) {
    font_t font = font_buffer[(uint8_t) (c - 'A')];

    for (uint16_t column = 0; column < font.width; column++) {
        for (uint16_t row = 0; row < font.height; row++) {
            uint16_t pixel = (row * font.width) + column;

            bool value = (font.hex[pixel / 8] >> (pixel % 8)) & 1;

            if (value) {
                waveshare_set_pixel(pass->canvas, (point_t) {.x = topleft.x + column, .y = topleft.y + row}, colour);
            }
        }
    }

    cbuff_write(pass->dirty_regions, &(rectangle_t) {
            .top_left = (point_t) {
                    .x = topleft.x,
                    .y = topleft.y
            },
            .bottom_right = (point_t) {
                    .x = topleft.x + font.width,
                    .y = topleft.y + font.height
            }
    });
}

void waveshare_clear(render_pass_t* pass, colour_t colour) {
    point_t point = (point_t) {};

    for(uint16_t x = 0; x < pass->canvas->width; x++) {
        for(uint16_t y = 0; y < pass->canvas->height; y++) {
            point.x = x;
            point.y = y;

            waveshare_set_pixel(pass->canvas, point, colour);
        }

        assert(pass->dirty_regions->size == 0);
    }

    cbuff_write(pass->dirty_regions, &(rectangle_t) {
        .top_left = (point_t) {
            .x = 0,
            .y = 0
        },
        .bottom_right = (point_t) {
            .x = pass->canvas->width,
            .y = pass->canvas->height
        }
    });
}

// start a rendering pass, indicating to the underlyign display that we are going to soon be
// sending updated dirty regions
void render_pass_init(canvas_t* canvas, render_pass_t* render) {
    render->canvas = canvas;
    render->dirty_regions = &dirty_regions;

    cbuff_clear(&dirty_regions);
}

// end the render pass, update the display with the buffer in the handle, updating the regions
// that are marked
extern void render_pass_end(render_pass_t* render) {
    size_t dirty_region_count = render->dirty_regions->size;

    if (dirty_region_count > 0) {
        if(render->canvas->display_impl->pre_render != NULL) {
            render->canvas->display_impl->pre_render();
        }

        canvas_t* canvas = render->canvas;

        rectangle_t regions[dirty_region_count];
        // read many should empty the buffer
        cbuff_readmany(render->dirty_regions, &regions, dirty_region_count);

        // these are the coordinates of the "dirty rectangle" of the display that needs to be redrawn
        // this is basically a bounding box around every dirtied region - it's relatively fast to compute
        // but will cause a lot of pixels that don't need to be redrawn to be redrawn
        uint16_t top_left_x = render->canvas->width;
        uint16_t top_left_y = render->canvas->height;
        uint16_t bottom_right_x = 0;
        uint16_t bottom_right_y = 0;

        for (size_t i = 0; i < dirty_region_count; i++) {
            rectangle_t region = regions[i];

            top_left_x = min(region.top_left.x, top_left_x);
            top_left_y = min(regions->top_left.y, top_left_y);

            bottom_right_x = max(region.bottom_right.x, bottom_right_x);
            bottom_right_y = max(regions->bottom_right.y, bottom_right_y);
        }

        canvas->display_impl->render_dirty_region(render->canvas->display_impl->buffer,
                                                  render->canvas->width,
                                                  render->canvas->height,
                                                  top_left_x,
                                                  top_left_y,
                                                  bottom_right_x,
                                                  bottom_right_y);


        if(render->canvas->display_impl->post_render != NULL) {
            render->canvas->display_impl->post_render();
        }

        // the read many should have done this, but just in case it didn't then clear it anyway
        cbuff_clear(&dirty_regions);
    }
}

void canvas_init(canvas_t* ops) {
    cbuff_init(&dirty_regions, &dirty_regions_raw, MAX_DIRTY_REGIONS, sizeof(rectangle_t));

    ops->height = WAVESHARE_PIXEL_HEIGHT;
    ops->width = WAVESHARE_PIXEL_WIDTH;

    ops->fill_rect = waveshare_fill_rect;
    ops->draw_char = waveshare_draw_char;
    ops->draw_line = waveshare_draw_line;
    ops->clear = waveshare_clear;
}

void canvas_destroy(canvas_t* ops) {

}
