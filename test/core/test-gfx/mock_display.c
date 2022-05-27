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


void mockdisplay_draw_line(display_handle_t* handle, point_t start, point_t end, uint8_t thickness, colour_t line_colour) {
    svg_line(handle->buffer, colour_lookup[line_colour], thickness, start.x, start.y, end.x, end.y);
}

void mockdisplay_fill_rect(display_handle_t* handle, point_t topleft, point_t bottomright, colour_t fill_colour) {
    svg_rectangle(handle->buffer, bottomright.x - topleft.x, bottomright.y - topleft.y, topleft.x, topleft.y, colour_lookup[fill_colour], NULL, 1, 0, 0);
}

void mockdisplay_draw_text(display_handle_t* surface, char c, point_t topleft, uint8_t size, colour_t colour) {

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