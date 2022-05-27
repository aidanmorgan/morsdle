//
// Created by aidan on 21/04/2022.
//

#include "waveshare_display.h"

#define MAX_FONT_BINARY_SIZE 255
#define MAX_DIRTY_REGIONS 32

static region_t _dirty_region[MAX_DIRTY_REGIONS];
static cbuff_t dirty_regions = (cbuff_t)&(struct cbuff){};

// struct that provides a base64 pixel by pixel encoding of each individual character
struct font_t {
    char c;

    uint16_t length;
    uint8_t* hex;
    uint16_t height;
    uint16_t width;
};

typedef struct font_t font_t;

// FIXME : This is super gross and needs to be done a different way, but it will do for the moment
// FIXME : should probably just have these files on the file system, with some structure to allow different sizes etc
// FIXME : but this will do for the moment until I can be bothered incorporating TinyFS or something similar.
//
// So this contains a base64 encoded binary, of 1-bit per pixel, row-first order
//
const font_t font_buffer[26] = {
        (font_t){
            .c = 'A',
            .length = 32,
            .hex = {0xFD, 0xFF, 0xFA , 0xFF , 0xF7 , 0x7F , 0xEF , 0xBF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF , 0xDF},
            .height=16,
            .width=16},
};


static colour_t colour_lookup [7] = {
        WAVESHARE_BLACK, //COLOUR_BLACK
        WAVESHARE_WHITE, //COLOUR_WHITE,
        WAVESHARE_GREEN, // COLOUR_GREEN,
        WAVESHARE_BLUE, // COLOUR_BLUE,
        WAVESHARE_RED, //COLOUR_RED,
        WAVESHARE_YELLOW, //COLOUR_YELLOW,
        WAVESHARE_ORANGE //COLOUR_ORANGE
};


static void waveshare_set_pixel(display_handle_t* surface, point_t point, colour_t colour) {
    if(point.x > surface->width || point.y > surface->height) {
        return;
    }

    uint16_t widthbyte =(surface->width % 2 == 0)? (surface->width / 2 ): (surface->width / 2 + 1);
    uint16_t pixel_idx = point.x / 8 + point.y * widthbyte;

    uint8_t pixel_data = surface->buffer[pixel_idx];

    if(colour == COLOUR_BLACK) {
        surface->buffer[pixel_idx] = pixel_data & ~(0x80 >> (point.x % 8));
    }
    else {
        pixel_data = pixel_data & (~(0xF0 >> ((point.x % 2)*4)));
        surface->buffer[pixel_idx] = pixel_data | ((colour_lookup[colour] << 4) >> ((point.x % 2)*4));
    }
}

void waveshare_draw_line(render_pass_t pass, point_t start, point_t end, uint8_t thickness, colour_t colour) {
    uint16_t current_x = start.x;
    uint16_t current_y = start.y;

    int dx = (int)end.x - (int)start.x >= 0 ? end.x - start.x : start.x - end.x;
    int dy = (int)end.y - (int)start.y <= 0 ? end.y - start.y : start.y - end.y;

    int x_direction = start.x < end.x ? 1 : -1;
    int y_direction = start.y < end.y ? 1 : -1;

    //Cumulative error
    int cumulativeerror = dx + dy;

    for (;;) {
        waveshare_set_pixel(pass->handle, (point_t){.x = current_x, .y = current_y}, colour);

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

    // FIXME : I am intentionally not dirtying the buffer here, as this is only called to render
    // FIXME : the initial grid, and that has already had the dirty region added to it
    // FIXME : When the dirty region coalescing is added we can put the dirtying here as it will be
    // FIXME : coalesced out pretty much immediately
}

void waveshare_fill_rect(render_pass_t pass, point_t topleft, point_t bottomright, colour_t fill_colour) {
    for(uint16_t y_idx = topleft.y; y_idx < bottomright.y; y_idx++) {
        // this seems kind of cheeky, but fill a rectangle by just drawing lines next to each other, over and over
        waveshare_draw_line(
                pass->handle,
                (point_t){.x = topleft.x, .y=y_idx},
                (point_t){.x = bottomright.x, .y =y_idx},
                1,
                fill_colour);
    }

    cbuff_write(pass->regions, &(region_t) {
            .top_left = (point_t) {
                    .x =,
                    .y =
            },
            .bottom_right = (point_t) {
                    .x =,
                    .y =
            }
    });
}


void waveshare_draw_char(render_pass_t  pass, char c, point_t topleft, uint8_t size, colour_t colour) {
    font_t font = font_buffer[(uint8_t)(c - 'A')];

    for(uint16_t column = 0; column < font.width; column++) {
        for(uint16_t row = 0; row < font.height; row++) {
            uint16_t pixel = (row * font.width) + column;

            bool value = (font.hex[pixel / 8] >> (pixel % 8)) & 1;

            if(value) {
                waveshare_set_pixel(pass->handle, (point_t) {.x = topleft.x + column, .y = topleft.y + row}, colour);
            }
        }
    }

    cbuff_write(pass->regions, &(region_t) {
            .top_left = (point_t) {
                    .x =,
                    .y =
            },
            .bottom_right = (point_t) {
                    .x =,
                    .y =
            }
    });
}



// start a rendering pass, indicating to the underlyign display that we are going to soon be
// sending updated dirty regions
void start_render_pass(display_handle_t* handle, render_pass_t render) {
    if(dirty_regions->size != 0) {
        // TODO: return an error here
        return;
    }

    render->handle = handle;
    render->regions = dirty_regions;
}

// end the render pass, update the display with the buffer in the handle, updating the regions
// that are marked
extern void end_render_pass(render_pass_t render) {
    // TODO : we really need to coalesce the regions into a smaller subset eventually, but that seems
    // TODO : somewhat overkill at the moment, but something to consider in the future
    size_t dirty_region_count = dirty_regions->size;

    region_t regions[dirty_region_count];
    cbuff_readmany(dirty_regions, &regions, dirty_region_count);

    for(size_t i = 0; i < dirty_region_count; i++) {
        region_t region = regions[i];

        render->handle->render_region_impl(render->handle->render_region_impl_cfg,
                                           render->handle->buffer,
                                           region.top_left.x,
                                           region.top_left.y,
                                           region.bottom_right.x,
                                           region.bottom_right.y);
    }
}

void display_init(display_t ops) {
    cbuff_init(dirty_regions, (void**)&_dirty_region, MAX_DIRTY_REGIONS, sizeof(region_t));

    ops->height = 600;
    ops->width = 448;

    ops->fill_rect=waveshare_fill_rect;
    ops->draw_char=waveshare_draw_char;
    ops->draw_line=waveshare_draw_line;
}

void display_destroy(display_t ops) {

}
