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
static display_handle_t* h_waveshare = &(display_handle_t) {};
static renderer_t h_renderer = &(renderer) {
    .height = 600,
    .width = 448
};

static wavesharestm_conf_t h_waveshare_conf = (struct wavesharestm_conf){
    .height = 600,
    .width = 448,

};

static display_t h_display = &(display_handle_t) {
        .height = 600,
        .width = 448,
        .buffer = imagebuffer,
        .render_region_impl_cfg = (void*)&h_waveshare_conf,
        .render_region_impl = wavesharestm_render_impl
};


int main(void) {
    // initialise the morse processor
    morse_init(h_morse);
    // initialise the  morsdle game engine
    morsdle_init_game(h_game);

    // plug the waveshare handle into the display container and then initialise
    h_display->handle = h_waveshare;
    display_init(h_display);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(h_display, h_renderer);

    // this is gross, but in an attempt to not mash the dependencies together I may have made a mistake
    // with the layering that means that the renderer has no way to actually tell the underlying hardware
    // to render a buffer...
    h_display->render = hackaround_render;

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

        // this probably only needs to happen once a second, but it will be slow to perform
        morsdle_game_event_t ev;
        render_pass_t render_pass;
        start_render_pass(h_waveshare, &render_pass);
        // process the game events and update the h_renderer h_display
        while (morsdle_has_events(h_game)) {
            if (morsdle_read_event(h_game, &ev)) {
                renderer_handle_event(h_display, h_renderer, &ev);
            }
        }

        end_render_pass(h_waveshare, &render_pass);

    }

    return 0;
}
