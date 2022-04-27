//
// Created by aidan on 21/04/2022.
//

#ifndef WAVESHARE_DISPLAY_H
#define WAVESHARE_DISPLAY_H


#include "morsdle.h"
#include "display.h"
#include "renderer.h"

struct display_handle_t  {
  uint32_t pretend_handle;
};

void waveshare_draw_line(display_handle_t* surface, point_t start, point_t end, uint8_t thickness, colour_t colour);
void waveshare_fill_rect(display_handle_t* surface, point_t topleft, point_t bottomright, colour_t fill_colour);
void waveshare_draw_char(display_handle_t* surface, char c, point_t topleft, uint8_t size, colour_t colour);


#endif //WAVESHARE_DISPLAY_H
