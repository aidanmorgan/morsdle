//
// Created by aidan on 21/04/2022.
//
#include "renderer.h"

static uint16_t min(uint16_t a, uint16_t b) {
    return a < b ? a : b;
}


void init_renderer(display_operations_t drawops, renderer_options_t renderopts) {
    renderopts->width = drawops->width;
    renderopts->height = drawops->height;

    renderopts->grid_line_width = 1;
    renderopts->cell_padding = 5;
    renderopts->grid_left_border = 5;
    renderopts->grid_right_border = 5;
    renderopts->grid_top_border = 5;
    renderopts->grid_bottom_border = 5;

    uint16_t constrained = min(renderopts->width, renderopts->height);

    // try and calculate the cell height/width based on the smaller dimension, as we have a 5x6 grid we need to be conscious of it
    if(constrained == renderopts->width || renderopts->width == renderopts->height) {
        // there are 2 paddings (left, right) and 6 lines and 12 insets

        renderopts->letter_cell_width = (renderopts->width - ((6 * renderopts->grid_line_width) + renderopts->grid_left_border + renderopts->grid_right_border)) / LETTERS_PER_WORD;
        renderopts->letter_cell_height = renderopts->letter_cell_width;
    }
    else {
        // TODO : implement this from a height-calculation perspective
    }

    renderopts->background_colour = COLOUR_WHITE;
    renderopts->grid_line_colour = COLOUR_BLACK;

    renderopts->cell_foreground_colours[LETTER_STATE_UNSET] = COLOUR_WHITE;
    renderopts->cell_foreground_colours[LETTER_STATE_SET] = COLOUR_BLACK;
    renderopts->cell_foreground_colours[LETTER_STATE_INVALID_LETTER] = COLOUR_BLACK;
    renderopts->cell_foreground_colours[LETTER_STATE_VALID_LETTER_INVALID_POSITION] = COLOUR_BLACK;
    renderopts->cell_foreground_colours[LETTER_STATE_VALID] = COLOUR_BLACK;

    renderopts->cell_background_colours[LETTER_STATE_UNSET] = COLOUR_WHITE;
    renderopts->cell_background_colours[LETTER_STATE_SET] = COLOUR_WHITE;
    renderopts->cell_background_colours[LETTER_STATE_INVALID_LETTER] = COLOUR_WHITE;
    renderopts->cell_background_colours[LETTER_STATE_VALID_LETTER_INVALID_POSITION] = COLOUR_ORANGE;
    renderopts->cell_background_colours[LETTER_STATE_VALID] = COLOUR_GREEN;
}

static void render_grid(renderer_options_t renderopts, display_operations_t drawops) {
    // draw the vertical lines
    for(uint8_t i = 0; i < LETTERS_PER_WORD + 1; i++) {
        drawops->draw_line(drawops->handle,
                           (point_t) {
                               .x =renderopts->grid_left_border + (i * renderopts->grid_line_width) + (i * renderopts->letter_cell_width),
                               .y = renderopts->grid_top_border
                           },
                           (point_t) {
                                   .x = renderopts->grid_left_border + (i * renderopts->grid_line_width) + (i * renderopts->letter_cell_width),
                                   .y = renderopts->grid_top_border + (WORDS_PER_GAME * renderopts->grid_line_width) + (WORDS_PER_GAME * renderopts->letter_cell_height)
                            },
                           renderopts->grid_line_width,
                           renderopts->grid_line_colour
                           );
    }

    // draw the horizontal lines
    for(uint8_t i = 0; i < WORDS_PER_GAME + 1; i++) {
        drawops->draw_line(drawops->handle,
                           (point_t) {
                                .x = renderopts->grid_left_border,
                                .y = renderopts->grid_top_border + (i*(renderopts->grid_line_width) + (i*renderopts->letter_cell_height))
                            },
                           (point_t) {
                                .x = renderopts->grid_left_border + (LETTERS_PER_WORD * renderopts->grid_line_width) + (LETTERS_PER_WORD * renderopts->letter_cell_width),
                                .y = renderopts->grid_top_border + (i*renderopts->grid_line_width) + (i*renderopts->letter_cell_height)
                            },
                            renderopts->grid_line_width,
                            renderopts->grid_line_colour);
    }
}

static void render_letter_cell(display_operations_t drawops, renderer_options_t renderopts, uint8_t letter_idx, uint8_t word_idx, char c, colour_t foreground_colour, colour_t background_colour) {
    uint16_t start_x = renderopts->grid_left_border + (letter_idx * renderopts->grid_line_width) + (letter_idx * renderopts->letter_cell_width);
    uint16_t start_y = renderopts->grid_top_border + (word_idx * renderopts->grid_line_width) + (word_idx * renderopts->letter_cell_height);

    // i dont want to lose this code, but I realised there is a LOT of wasted drawing instructions that aren't really needed, as long as we keep
    // filling the "inside" of the grid we don't need to actually redraw it all the time, so I've moved the grid drawing into the clear operation
    // and will now only fill the inside of the rectangle (and draw the letter if need be) rather than redrawing the grid cells
    /*
    // top line
    drawops->draw_line(drawops->handle,
                       (point_t){start_x, start_y},
                       (point_t){start_x + renderopts->letter_cell_width, start_y},
                       renderopts->grid_line_width,
                       renderopts->grid_line_colour
                       );
    // left line
    drawops->draw_line(drawops->handle,
                       (point_t){start_x, start_y},
                       (point_t){start_x, start_y + renderopts->letter_cell_height},
                       renderopts->grid_line_width,
                       renderopts->grid_line_colour
    );

    // right line
    drawops->draw_line(drawops->handle,
                       (point_t){start_x + renderopts->letter_cell_width, start_y},
                       (point_t){start_x + renderopts->letter_cell_width, start_y + renderopts->letter_cell_height},
                       renderopts->grid_line_width,
                       renderopts->grid_line_colour
    );

    // right line
    drawops->draw_line(drawops->handle,
                       (point_t){start_x, start_y+ renderopts->letter_cell_height},
                       (point_t){start_x + renderopts->letter_cell_width, start_y + renderopts->letter_cell_height},
                       renderopts->grid_line_width,
                       renderopts->grid_line_colour
    );

     */
    // draw a rect that is grid_line_width smaller in all directons in the actual colour we want the background to be
    drawops->fill_rect(drawops->handle,
                       (point_t){start_x + renderopts->grid_line_width, start_y + renderopts->grid_line_width},
                       (point_t){start_x + renderopts->letter_cell_width, start_y + renderopts->letter_cell_height},
                       background_colour
                   );

    if(c != NULL_CHAR) {
         start_x = renderopts->grid_left_border + (letter_idx * renderopts->grid_line_width) + (letter_idx * renderopts->letter_cell_width) + (letter_idx * renderopts->cell_padding * 2);
         start_y = renderopts->grid_top_border + (word_idx * renderopts->grid_line_width) + (word_idx * renderopts->letter_cell_width) + (word_idx * renderopts->cell_padding * 2);

         drawops->draw_char(drawops->handle,
                            c,
                            (point_t){start_x, start_y},
                            renderopts->font_size,
                            foreground_colour
                            );
    }
}

void renderer_handle_event(display_operations_t drawops, renderer_options_t renderopts, morsdle_game_event_t *event) {
    switch (event->type) {
        case EVENT_GAME_CREATED: {
            renderer_clear(drawops, renderopts);
            break;
        }

        case EVENT_LETTER_ADDED: {
            render_letter_cell(drawops, renderopts,
                               event->letter->x,
                               event->letter->y,
                               event->letter->letter,
                               renderopts->cell_foreground_colours[LETTER_STATE_SET],
                               renderopts->cell_background_colours[LETTER_STATE_SET]);
            break;
        }

        case EVENT_LETTER_REMOVED: {
            // we need to go through and blank out the contents that was there and replace it with something else
            render_letter_cell(drawops, renderopts, event->letter->x, event->letter->y, NULL_CHAR,
                               renderopts->background_colour, renderopts->background_colour);
            break;
        }

        case EVENT_WORD_STARTED: {
            break;
        }

        case EVENT_WORD_COMPLETED: {
            // go through and change the entire row of colours now
            for(uint8_t l = 0; l < LETTERS_PER_WORD; l++) {
                morsdle_letter_t letter = event->word->letters[l];

                render_letter_cell(drawops, renderopts,
                                   letter.x,
                                   letter.y,
                                   letter.letter,
                                   renderopts->cell_foreground_colours[letter.state],
                                   renderopts->cell_background_colours[letter.state]);
            }

            break;
        }

        case EVENT_GAME_COMPLETED: {
            break;
        }

        default: {
            // do nothing, we don't care really
            break;
        }

    }
}

void renderer_clear(display_operations_t drawops, renderer_options_t renderopts) {
    // fill the background with the default background colour
    drawops->fill_rect(drawops->handle, (point_t){0,0}, (point_t){drawops->width, drawops->height}, renderopts->background_colour);
    // now render the grid only
    render_grid(renderopts, drawops);
}

