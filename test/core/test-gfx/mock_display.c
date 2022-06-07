//
// Created by aidan on 21/04/2022.
//

#include "mock_display.h"

#define MAX_DIRTY_REGIONS 32

static rectangle_t _dirty_region[MAX_DIRTY_REGIONS];
static cbuff_t dirty_regions = (cbuff_t) {};


static const char* mock_buffer_to_waveshare_lookup[7] = {
        "black",
        "white",
        "green",
        "blue",
        "red",
        "yellow",
        "orange"
};


void mockdisplay_draw_line(render_pass_t* render,display_impl_t * display, point_t start, point_t end, uint8_t thickness, colour_t line_colour) {
    svg_line(render->display->buffer, mock_buffer_to_waveshare_lookup[line_colour], thickness, start.x, start.y, end.x, end.y);
    cbuff_write(render->dirty_regions, &(rectangle_t){
        .top_left = start,
        .bottom_right = end
    });
}

void mockdisplay_fill_rect(render_pass_t* render,display_impl_t * display, point_t topleft, point_t bottomright, colour_t fill_colour) {
    svg_rectangle(render->display->buffer, bottomright.x - topleft.x, bottomright.y - topleft.y, topleft.x, topleft.y, mock_buffer_to_waveshare_lookup[fill_colour], NULL, 1, 0, 0);
    cbuff_write(render->dirty_regions, &(rectangle_t){
            .top_left = topleft,
            .bottom_right = bottomright
    });

}

void mockdisplay_clear(render_pass_t* render, display_impl_t * display,colour_t colour) {
    cbuff_write(render->dirty_regions, &(rectangle_t) {
            .top_left = (point_t) {.x = 0, .y = 0},
            .bottom_right = (point_t) {.x = render->canvas->width, .y = render->canvas->height}
    });
}

void mockdisplay_draw_text(render_pass_t *render,display_impl_t * display, char c, point_t topleft, uint8_t size, colour_t colour) {

}

void mockdisplay_write_buffer(display_impl_t* handle, char* filename) {
    svg_save(handle->buffer, filename);
}


#define MOCK_DISPLAY_WIDTH 448
#define MOCK_DISPLAY_HEIGHT 600

void canvas_init(canvas_t* ops) {
    ops->height = MOCK_DISPLAY_HEIGHT;
    ops->width = MOCK_DISPLAY_WIDTH;

    ops->fill_rect=mockdisplay_fill_rect;
    ops->draw_char=mockdisplay_draw_text;
    ops->draw_line=mockdisplay_draw_line;
    ops->clear=mockdisplay_clear;

    cbuff_init(&dirty_regions, _dirty_region, MAX_DIRTY_REGIONS, sizeof(rectangle_t));
}

void canvas_destroy(canvas_t* ops) {
}

void render_pass_init(render_pass_t* render) {
    render->dirty_regions = &dirty_regions;
}

void render_pass_end(render_pass_t* render) {
    cbuff_clear(&dirty_regions);
}