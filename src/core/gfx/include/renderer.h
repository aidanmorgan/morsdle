//
// Created by aidan on 21/04/2022.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "drawing_ops.h"
#include "morsdle.h"

typedef struct {
    uint16_t height;
    uint16_t width;

    uint8_t grid_line_width;    // the width of the line to draw (in pixels)

    uint8_t grid_left_border;   // the offset of the start of the grid from the left edge of the screen
    uint8_t grid_right_border;  // the offset of the end of the grid from the right edge of the screen

    uint8_t grid_top_border;    // the offset of the start of the grid from the top edge of the screen
    uint8_t grid_bottom_border; // the offset of the end of the grid from the bottom edge of the screen

    uint16_t letter_cell_width;  // the width of the cell that a letter is within
    uint16_t letter_cell_height; // the height of the cell that a letter is within

    uint8_t cell_padding;  // the padding for inside a letter cell before the letter is drawn

    uint8_t font_size;


    colour_t background_colour;
    colour_t grid_line_colour;

    // maps to letter state as an index
    colour_t cell_background_colours[5];
    colour_t cell_foreground_colours[5];

} renderer_opts;
typedef renderer_opts* renderer_opts_t;

void init_renderer(renderer_opts_t renderopts, drawing_ops_t drawops);

void handle_event(renderer_opts_t renderopts, drawing_ops_t drawops, morsdle_game_change_event_t event);
void clear(renderer_opts_t renderopts, drawing_ops_t drawops);

#endif //RENDERER_H
