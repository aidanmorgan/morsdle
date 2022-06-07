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

static imagebuffer_colour_t display_to_buffer_lookup[7] = {
        IMAGEBUFFER_BLACK,
        IMAGEBUFFER_CLEAR,
        IMAGEBUFFER_GREEN,
        IMAGEBUFFER_INVALID,
        IMAGEBUFFER_INVALID,
        IMAGEBUFFER_INVALID,
        IMAGEBUFFER_ORANGE
};

void waveshare_draw_line_impl(render_pass_t* pass,display_impl_t * display, point_t start, point_t end, uint8_t thickness, colour_t colour, bool updateDirtyRegions) {
    imagebuffer_t * buffer = display->buffer;

    uint16_t current_x = start.x;
    uint16_t current_y = start.y;

    int dx = (int) end.x - (int) start.x >= 0 ? end.x - start.x : start.x - end.x;
    int dy = (int) end.y - (int) start.y <= 0 ? end.y - start.y : start.y - end.y;

    int x_direction = start.x < end.x ? 1 : -1;
    int y_direction = start.y < end.y ? 1 : -1;

    //Cumulative error
    int cumulativeerror = dx + dy;

    for (;;) {
        imagebuffer_setpixel(buffer, current_x, current_y, display_to_buffer_lookup[colour]);

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

void waveshare_draw_line(render_pass_t* pass, display_impl_t * display, point_t start, point_t end, uint8_t thickness, colour_t colour) {
    waveshare_draw_line_impl(pass, display, start, end, thickness, colour, true);
}


void waveshare_fill_rect(render_pass_t* pass, display_impl_t * display,  point_t topleft, point_t bottomright, colour_t fill_colour) {
    imagebuffer_t * buffer = display->buffer;

    for(uint16_t x = topleft.x; x < topleft.x; x++) {
        for(uint16_t y = bottomright.y; y < bottomright.y; y++) {
            imagebuffer_setpixel(buffer, x, y, display_to_buffer_lookup[fill_colour]);
        }
    }

    cbuff_write(pass->dirty_regions, &(rectangle_t) {
            .top_left = topleft,
            .bottom_right = bottomright
    });
}


void waveshare_draw_char(render_pass_t* pass,display_impl_t * display, char c, point_t topleft, uint8_t size, colour_t colour) {
    imagebuffer_t * buffer = display->buffer;

    font_t font = font_buffer[(uint8_t) (c - 'A')];

    for (uint16_t column = 0; column < font.width; column++) {
        for (uint16_t row = 0; row < font.height; row++) {
            uint16_t pixel = (row * font.width) + column;

            bool value = (font.hex[pixel / 8] >> (pixel % 8)) & 1;

            if (value) {
                imagebuffer_setpixel(buffer, topleft.x + column, topleft.y + row, display_to_buffer_lookup[colour]);
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

void waveshare_clear(render_pass_t* pass, display_impl_t * display,colour_t colour) {
    imagebuffer_t * buffer = pass->display->buffer;

    for(uint16_t x = 0; x < pass->canvas->width; x++) {
        for(uint16_t y = 0; y < pass->canvas->height; y++) {
            imagebuffer_setpixel(buffer, x, y, display_to_buffer_lookup[colour]);
        }
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
void render_pass_init(render_pass_t* render) {
    render->dirty_regions = &dirty_regions;
    cbuff_clear(&dirty_regions);
}

// end the render pass, update the display with the buffer in the handle, updating the regions
// that are marked
extern void render_pass_end(render_pass_t* render) {
    size_t dirty_region_count = render->dirty_regions->size;

    if (dirty_region_count > 0) {
        if(render->display->pre_render != NULL) {
            render->display->pre_render(render->display->buffer);
        }

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

        render->display->render_dirty_region(render->display->buffer,
                                                  render->canvas->width,
                                                  render->canvas->height,
                                                  top_left_x,
                                                  top_left_y,
                                                  bottom_right_x,
                                                  bottom_right_y);


        if(render->display->post_render != NULL) {
            render->display->post_render();
        }

        // the read many should have done this, but just in case it didn't then clear it anyway
        cbuff_clear(&dirty_regions);
    }
}




void canvas_init(canvas_t* ops) {
    cbuff_init(&dirty_regions, &dirty_regions_raw, MAX_DIRTY_REGIONS, sizeof(rectangle_t));

    ops->height = WAVESHARE_PIXEL_HEIGHT;
    ops->width = WAVESHARE_PIXEL_WIDTH;
}

void canvas_destroy(canvas_t* ops) {

}
