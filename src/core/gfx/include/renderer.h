//
// Created by aidan on 21/04/2022.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "display.h"
#include "morsdle.h"

typedef enum {
    // whether the game should accept each letter from the user and then render it on the display (easy mode)
    MORSDLE_GAME_SINGLE_LETTER,
    // the game waits until the user has inputted a complete word before rendering on the display (hard mode)
    MORSDLE_GAME_WHOLE_WORD
} morsdle_game_mode_t;

typedef struct {
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

    morsdle_game_mode_t game_mode;
} renderer_t;

void renderer_init(canvas_t* canvas, renderer_t* renderopts);

void renderer_handle_event(canvas_t* canvas, renderer_t* renderopts, render_pass_t* pass, morsdle_game_event_t *event);
void renderer_clear(canvas_t* canvas, renderer_t * renderopts, render_pass_t* pass);

#endif //RENDERER_H
