//
// Created by aidan on 21/04/2022.
//
#include "renderer.h"

void init_renderer(renderer_opts_t renderopts, drawing_ops_t drawops) {
    renderopts->width = drawops->width;
    renderopts->height = drawops->height;

    renderopts->grid_line_width = 1;
    renderopts->cell_padding = 5;
    renderopts->grid_left_border = 5;
    renderopts->grid_right_border = 5;
    renderopts->grid_top_border = 5;
    renderopts->grid_bottom_border = 5;

    // there are 2 paddings (left, right) and 6 lines and 12 insets
    renderopts->letter_cell_width = renderopts->width - (6*renderopts->grid_line_width) + renderopts->grid_left_border + renderopts->grid_right_border;
    renderopts->letter_cell_height = renderopts->letter_cell_width;

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

static void render_letter_cell(renderer_opts_t renderopts, drawing_ops_t drawops, uint8_t l, uint8_t w, char c, colour_t foreground_colour, colour_t background_colour) {
    uint16_t start_x = renderopts->grid_left_border + (l * renderopts->grid_line_width) + (l * renderopts->letter_cell_width);
    uint16_t start_y = renderopts->grid_top_border + (w * renderopts->grid_line_width) + (w * renderopts->letter_cell_height);

    // fill a rect that is the correct size in the line colour
    drawops->fill_rect(drawops->handle,
                       (point_t){start_x, start_y},
                       (point_t){start_x + renderopts->letter_cell_width, start_y + renderopts->letter_cell_height},
                       renderopts->grid_line_colour
                   );

    // draw a rect that is grid_line_width smaller in all directons in the actual colour we want the background to be
    drawops->fill_rect(drawops->handle,
                       (point_t){start_x + renderopts->grid_line_width, start_y + renderopts->grid_line_width},
                       (point_t){start_x + renderopts->letter_cell_width - renderopts->grid_line_width, start_y + renderopts->letter_cell_height - renderopts->grid_line_width},
                       background_colour
                   );

    // voila- we have a line

    if(c != (char)0) {
         start_x = renderopts->grid_left_border + (l * renderopts->grid_line_width) + (l * renderopts->letter_cell_width) + (l * renderopts->cell_padding * 2);
         start_y = renderopts->grid_top_border + (w * renderopts->grid_line_width) + (w * renderopts->letter_cell_width) + (w * renderopts->cell_padding * 2);

         drawops->draw_text(drawops->handle,
                            c,
                            (point_t){start_x, start_y},
                            renderopts->font_size,
                            foreground_colour
                            );
    }
}

void handle_event(renderer_opts_t renderopts, drawing_ops_t drawops, morsdle_game_change_event_t event) {
    switch (event->type) {
        case EVENT_GAME_CREATED: {
            clear(renderopts, drawops);

            // draw each rectangle on the screen, it's a new game after all
            for(uint8_t l; l < LETTERS_PER_WORD; l++) {
                for(uint8_t w; w < WORDS_PER_GAME; w++) {
                    render_letter_cell(renderopts, drawops, l, w, (char)0, renderopts->background_colour, renderopts->background_colour);
                }
            }

            break;
        }

        case EVENT_LETTER_ADDED: {
            render_letter_cell(renderopts, drawops,
                               event->letter->x,
                               event->letter->y,
                               event->letter->letter,
                               renderopts->cell_foreground_colours[LETTER_STATE_SET],
                               renderopts->cell_background_colours[LETTER_STATE_SET]);
            break;
        }

        case EVENT_LETTER_REMOVED: {
            // we need to go through and blank out the contents that was there and replace it with something else
            render_letter_cell(renderopts, drawops, event->letter->x, event->letter->y, (char)0, renderopts->background_colour, renderopts->background_colour);
            break;
        }

        case EVENT_WORD_STARTED: {
            break;
        }

        case EVENT_WORD_COMPLETED: {
            // go through and change the entire row of colours now
            for(uint8_t l; l < LETTERS_PER_WORD; l++) {
                render_letter_cell(renderopts, drawops,
                                   event->letter->x,
                                   event->word->y,
                                   event->word->letters[l]->letter,
                       renderopts->cell_foreground_colours[event->word->letters[l]->state],
                       renderopts->cell_background_colours[event->word->letters[l]->state]);
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

void clear(renderer_opts_t renderopts, drawing_ops_t drawops) {
    // fill the background with the default background colour
    drawops->fill_rect(drawops->handle, (point_t){0,0}, (point_t){drawops->width, drawops->height}, renderopts->background_colour);

}
