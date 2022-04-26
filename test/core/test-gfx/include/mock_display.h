//
// Created by aidan on 21/04/2022.
//

#ifndef MOCK_DISPLAY_H
#define MOCK_DISPLAY_H

#include <stdint.h>
#include <math.h>
#include "display.h"
#include "allocations.h"
#include "svg.h"

struct display_handle_t {
    svg * buffer;
    uint32_t height;
    uint32_t width;
};

void mockdisplay_draw_line(display_handle_t* surface, point_t start, point_t end, uint8_t thickness, colour_t colour);
void mockdisplay_fill_rect(display_handle_t* surface, point_t topleft, point_t bottomright, colour_t fill_colour);
void mockdisplay_draw_text(display_handle_t* surface, char c, point_t topleft, uint8_t size, colour_t colour);

void mockdisplay_write_buffer(display_handle_t* handle, char* filename);

#endif //MOCK_DISPLAY_H
