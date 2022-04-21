//
// Created by aidan on 21/04/2022.
//

#ifndef WAVESHARE_DISPLAY_H
#define WAVESHARE_DISPLAY_H


#include "morsdle.h"
#include "drawing_ops.h"

typedef struct  {
  uint32_t pretend_handle;
} waveshare_display_handle_t;

void waveshare_draw_line(void* surface, point_t start, point_t end, uint8_t thickness, colour_t colour);
void waveshare_fill_rect(void* surface, point_t topleft, point_t bottomright, colour_t fill_colour);
void waveshare_draw_text(void* surface, char c, point_t topleft, uint8_t size, colour_t colour);


#endif //WAVESHARE_DISPLAY_H
