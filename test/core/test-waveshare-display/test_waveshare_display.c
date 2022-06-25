//
// Created by Aidan Morgan on 20/6/2022.
//
#include "unity.h"
#include "cbuff.h"
#include "canvas.h"
#include "renderer.h"
#include "waveshare_canvas.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

static void no_op_init(){

}

static void no_op_render_region(imagebuffer_t *ptr, uint16_t i, uint16_t i1, uint16_t i2, uint16_t i3, uint16_t i4) {

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

void process_events(canvas_t *canvas, display_impl_t * display, renderer_t *renderer, morsdle_game_t *game) {
    render_pass_t *pass = &(render_pass_t) {
        .canvas = canvas,
        .display = display
    };
    render_pass_init(pass);

    while(morsdle_has_events(game)) {
        morsdle_game_event_t ev;
        morsdle_read_event(game, &ev);

        renderer_handle_event(canvas, renderer, pass, &ev);
    }

    render_pass_end(pass);
}

void write_ppm_image(imagebuffer_t *buffer, const char * filename) {// http://www.cs.cmu.edu/~maxwell/misc/vascHelpPages/ppmFormat.html
    FILE * file = fopen(filename, "w");
    fprintf(file, "P3\n");
    fprintf(file, "600 448\n");
    fprintf(file, "255\n");

    for(uint16_t y = 0; y < 448; y++) {
        for(uint16_t x = 0; x < 600; x++) {
            colour_t pixel;
            imagebuffer_getpixel(buffer, x, y, &pixel);

            if(x > 0) {
                fprintf(file, "   ");
            }

            fprintf(file, "%u %u %u", red_table[pixel], green_table[pixel], blue_table[pixel]);
        }

        fprintf(file, "\n");
    }

    fflush(file);
    fclose(file);
}


void create_filename(char *filename, uint8_t len, uint8_t num, const char *suffix) {
    memset(filename, 0, len);
    snprintf(filename, len, "%s%d%s%s%s", "morsdle-board-", num, "-", suffix, ".ppm");
}

void test_full_board(uint16_t width, uint16_t height, char* suffix) {
    imagebuffer_t buffer = (imagebuffer_t) {  };
    imagebuffer_init(&buffer, width, height);

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
    canvas_init(canvas, width > height ? ROTATION_NONE : ROTATION_NINETY);

    renderer_t* renderer = &(renderer_t) {};
    renderer_init(renderer, width, height);

    morsdle_game_t* game = &(morsdle_game_t) {
        .auto_submit = false,
    };
    morsdle_init_game(game, "RATIO");

    morsdle_add_letter(game, 'B');
    morsdle_add_letter(game, 'U');
    morsdle_add_letter(game, 'T');
    morsdle_add_letter(game, 'T');
    morsdle_add_letter(game, 'S');
    morsdle_submit_word(game);

    const uint8_t filename_length = strlen(suffix) + strlen("morsdle-board-0-.ppm") + 1;
    char filename[filename_length];
    create_filename(filename, filename_length, 1, suffix);

    process_events(canvas, display, renderer, game);
    write_ppm_image(&buffer, filename);


    morsdle_add_letter(game, 'H');
    morsdle_add_letter(game, 'E');
    morsdle_add_letter(game, 'R');
    morsdle_add_letter(game, 'O');
    morsdle_add_letter(game, 'S');
    morsdle_submit_word(game);
    process_events(canvas, display, renderer, game);

    create_filename(filename, filename_length, 2, suffix);
    write_ppm_image(&buffer, filename);


    morsdle_add_letter(game, 'E');
    morsdle_add_letter(game, 'L');
    morsdle_add_letter(game,'B');
    morsdle_add_letter(game, 'O');
    morsdle_add_letter(game, 'W');
    morsdle_submit_word(game);
    process_events(canvas, display, renderer, game);

    create_filename(filename, filename_length, 3, suffix);
    write_ppm_image(&buffer, filename);


    morsdle_add_letter(game, 'R');
    morsdle_add_letter(game, 'A');
    morsdle_add_letter(game, 'T');
    morsdle_add_letter(game, 'I');
    morsdle_add_letter(game, 'O');
    morsdle_submit_word(game);
    process_events(canvas,  display, renderer, game);

    create_filename(filename, filename_length, 4, suffix);
    write_ppm_image(&buffer, filename);
}

void test_full_board_horizontal(void) {
    test_full_board(600, 448, "horizontal");
}

void test_full_board_vertical(void) {
    test_full_board(448, 600, "vertical");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_full_board_vertical);
    RUN_TEST(test_full_board_horizontal);

    return UNITY_END();
}
