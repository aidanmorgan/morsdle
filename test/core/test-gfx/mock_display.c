//
// Created by aidan on 21/04/2022.
//

#include "mock_display.h"

static const char* colour_lookup[7] = {
        "black",
        "white",
        "green",
        "blue",
        "red",
        "yellow",
        "orange"
};


void mockdisplay_draw_line(render_pass_t render, point_t start, point_t end, uint8_t thickness, colour_t line_colour) {
    svg_line(render->handle->buffer, colour_lookup[line_colour], thickness, start.x, start.y, end.x, end.y);
}

void mockdisplay_fill_rect(render_pass_t render, point_t topleft, point_t bottomright, colour_t fill_colour) {
    svg_rectangle(render->handle->buffer, bottomright.x - topleft.x, bottomright.y - topleft.y, topleft.x, topleft.y, colour_lookup[fill_colour], NULL, 1, 0, 0);
}

void mockdisplay_draw_text(render_pass_t render, char c, point_t topleft, uint8_t size, colour_t colour) {

}

void mockdisplay_write_buffer(display_handle_t* handle, char* filename) {
    svg_save(handle->buffer, filename);
}


#define MOCK_DISPLAY_WIDTH 448
#define MOCK_DISPLAY_HEIGHT 600

void display_init(display* ops) {
    ops->height = MOCK_DISPLAY_HEIGHT;
    ops->width = MOCK_DISPLAY_WIDTH;

    ops->handle = (display_handle_t*)INLINE_MALLOC(display_handle_t, .buffer = svg_create(MOCK_DISPLAY_WIDTH, MOCK_DISPLAY_HEIGHT), .width = MOCK_DISPLAY_WIDTH, .height = MOCK_DISPLAY_HEIGHT);

    ops->fill_rect=mockdisplay_fill_rect;
    ops->draw_char=mockdisplay_draw_text;
    ops->draw_line=mockdisplay_draw_line;
}

void display_destroy(display_t ops) {
    svg_free(ops->handle->buffer);
}

#define MAX_DIRTY_REGIONS 32

static region_t _dirty_region[MAX_DIRTY_REGIONS];
static cbuff_t dirty_regions = (cbuff_t) &(struct cbuff) {};

void render_pass_init(display_handle_t *handle, render_pass_t render) {
    render->handle = handle;
    render->dirty_regions = dirty_regions;
}

void render_pass_end(render_pass_t render) {
    cbuff_clear(dirty_regions);
}