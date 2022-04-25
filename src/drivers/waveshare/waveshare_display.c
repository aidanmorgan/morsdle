//
// Created by aidan on 21/04/2022.
//

#include "waveshare_display.h"

void init_drawing_ops(display_operations_t ops) {
    ops->handle = INLINE_MALLOC(display_handle_t, .pretend_handle = 5555555);
    ops->height = 600;
    ops->width = 448;

    ops->fill_rect=waveshare_fill_rect;
    ops->draw_text=waveshare_draw_text;
    ops->draw_line=waveshare_draw_line;
}

void waveshare_draw_line(display_handle_t* surface, point_t start, point_t end, uint8_t thickness, colour_t colour) {

}
void waveshare_fill_rect(display_handle_t* surface, point_t topleft, point_t bottomright, colour_t fill_colour) {

}
void waveshare_draw_text(display_handle_t* surface, char c, point_t topleft, uint8_t size, colour_t colour) {

}


void destroy_drawing_ops(display_operations_t ops) {

}
