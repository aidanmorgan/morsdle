//
// Created by Aidan Morgan on 20/6/2022.
//
#include "unity.h"
#include "cbuff.h"
#include "display.h"
#include "renderer.h"
#include "waveshare_display.h"
#include "svg.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

static void no_op_init(){

}

static void no_op_render_region(imagebuffer_t *ptr, uint16_t i, uint16_t i1, uint16_t i2, uint16_t i3, uint8_t i4) {

}

static uint8_t red_table[4] = {
    255,
    255,
    0,
    0
};
static uint8_t green_table[4] = {
    255,
    170,
    128,
    0
};
static uint8_t blue_table[4] = {
    255,
    0,
    0,
    0
};

void test_full_board(void) {
    imagebuffer_t buffer = (imagebuffer_t) {  };
    imagebuffer_init(&buffer, 600, 448);

    display_impl_t* display = &(display_impl_t) {
        .buffer = &buffer,
        .render_region = no_op_render_region,
        .init = no_op_init
    };

    canvas_t* canvas = &(canvas_t){
        .fill_rect = waveshare_fill_rect,
        .draw_line = waveshare_draw_line,
        .clear = waveshare_clear,
        .draw_char = waveshare_draw_char
    };
    canvas_init(canvas);

    renderer_t* renderer = &(renderer_t) {};
    renderer_init(renderer, 600, 448);

    morsdle_game_t* game = &(morsdle_game_t) {
        .auto_submit = true,
    };
    morsdle_init_game(game, "ratio");

    render_pass_t * pass = &(render_pass_t) {
        .canvas = canvas,
        .display = display
    };
    render_pass_init(pass);

    morsdle_add_letter(game, 'b');
    morsdle_add_letter(game, 'u');
    morsdle_add_letter(game, 't');
    morsdle_add_letter(game, 't');
    morsdle_add_letter(game, 's');

    morsdle_add_letter(game, 'h');
    morsdle_add_letter(game, 'e');
    morsdle_add_letter(game, 'r');
    morsdle_add_letter(game, 'o');
    morsdle_add_letter(game, 's');

    morsdle_add_letter(game, 'e');
    morsdle_add_letter(game, 'l');
    morsdle_add_letter(game,'b');
    morsdle_add_letter(game, 'o');
    morsdle_add_letter(game, 'w');

    morsdle_add_letter(game, 'r');
    morsdle_add_letter(game, 'a');
    morsdle_add_letter(game, 't');
    morsdle_add_letter(game, 'i');
    morsdle_add_letter(game, 'o');

    while(morsdle_has_events(game)) {
        morsdle_game_event_t ev;
        morsdle_read_event(game, &ev);

        renderer_handle_event(canvas, renderer, pass, &ev);
    }

    // http://www.cs.cmu.edu/~maxwell/misc/vascHelpPages/ppmFormat.html
    FILE * file = fopen("morsdle-board.ppm", "w");
    fprintf(file, "P3\n");
    fprintf(file, "600 448\n");
    fprintf(file, "255\n");

    for(uint16_t x = 0; x < 600; x++) {
        for(uint16_t y = 0; y < 448; y++) {
            imagebuffer_colour_t pixel;
            imagebuffer_getpixel(&buffer, x, y, &pixel);

            if(y > 0) {
                fprintf(file, "  ");
            }

            fprintf(file, "%u %u %u", red_table[pixel], green_table[pixel], blue_table[pixel]);
        }

        fprintf(file, "\n");
    }

    fflush(file);
    fclose(file);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_full_board);

    return UNITY_END();
}
