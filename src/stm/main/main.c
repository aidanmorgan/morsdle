#include "stm32l4xx_hal_conf.h"
#include "morse.h"
#include "cbuff.h"
#include "morsdle.h"
#include "renderer.h"
#include "waveshare_display.h"
#include "waveshare_stm.h"

static uint8_t imagebuffer[600 * 448];

static morse_t h_morse = &(struct morse) {};
static morsdle_game_t* h_game = &(morsdle_game_t) {.word = "ratio" };

static renderer_t h_renderer = &(renderer) {
    .game_mode = MORSDLE_GAME_SINGLE_LETTER
};

static wavesharestm_conf_t h_waveshare_stm = &(struct wavesharestm_conf){
    .busy = (struct wavesharestm_pin_conf) {
        .port = 0,
        .pin = 0
    },
    .cs = (struct wavesharestm_pin_conf) {
            .port = 0,
            .pin = 0
    },
    .dc = (struct wavesharestm_pin_conf) {
            .port = 0,
            .pin = 0
    },
    .rst = (struct wavesharestm_pin_conf) {
            .port = 0,
            .pin = 0
    }
};

// this is the waveshare implementation of the handle
static display_impl_t display_handle = (display_impl_t) {
        .buffer = imagebuffer,
        .waveshare_conf = (void*)&h_waveshare_stm,
        .waveshare_render_region = wavesharestm_render_impl
};

static canvas_t h_display = &(struct canvas) {
    .display_impl = &display_handle,
    .height = 600,
    .width = 448
};

int main(void) {
    // initialise the morse processor
    morse_init(h_morse);
    // initialise the  morsdle game engine
    morsdle_init_game(h_game);

    // plug the waveshare handle into the display container and then initialise
    canvas_init(h_display);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(h_display, h_renderer);

    while(1) {
        // this needs to happen every MORSE_DIT_MS / 2 ms.
        morse_append_signal(h_morse, SIGNAL_VOID, 0);

        // this probably only needs to happen every 2 * MORSE_DIT ms (dit to high, dit to low)
        bool converted = morse_convert(h_morse, 0);

        if(converted) {
            char result;
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            if (morse_decode(h_morse, &result)) {
                morsdle_add_letter(h_game, result);
            }
        }

        // are there any game events that may require processing
        if(morsdle_has_events(h_game)) {
            morsdle_game_event_t ev;

            // we aren't going to bother drawing unless there's an event, and even then they may not dirty the display
            // so collect any dirty regions into a render_pass and then see if we actually need to redraw.
            struct render_pass render_pass;
            render_pass_init(h_display->display_impl, &render_pass);

            while (morsdle_has_events(h_game)) {
                if (morsdle_read_event(h_game, &ev)) {
                    renderer_handle_event(h_display, h_renderer, &render_pass, &ev);
                }
            }

            render_pass_end(&render_pass);
        }

    }

    return 0;
}
