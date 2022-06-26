#include <stdint.h>
#include "morsdle_main.h"
#include "main.h"
#include "words.h"
#include "morse.h"
#include "morsdle.h"
#include "renderer.h"
#include "stm_flash_read.h"
#include "waveshare_canvas.h"
#include "waveshare_api.h"

static void waveshare_display_init(void) {
    waveshareapi_moduleinit();
    waveshareapi_reset();
    waveshareapi_init();
}

static morse_t h_morse = (morse_t) {};
static morsdle_game_t h_game = (morsdle_game_t) {
        .auto_submit = true
};
static renderer_t h_renderer = (renderer_t) {
        .game_mode = MORSDLE_GAME_WHOLE_WORD
};
static imagebuffer_t h_image_buffer = (imagebuffer_t) {};
// this is the waveshare implementation of the handle
static display_impl_t h_display = (display_impl_t) {
        .buffer = &h_image_buffer,
        .render_region = waveshareapi_render_region,
//        .pre_render = waveshareapi_wake,
//        .post_render = waveshareapi_sleep,
        .init = waveshare_display_init
};
static canvas_t h_canvas = (canvas_t) {
        .fill_rect = waveshare_fill_rect,
        .draw_line = waveshare_draw_line,
        .clear = waveshare_clear,
        .draw_char = waveshare_draw_char
};

static flash_cfg_t* h_flash_cfg;

static const char *morsdle_random_word() {
    uint32_t base;

    // TODO : implement flash dictionary loading here
    if(HAL_RNG_GenerateRandomNumber(&hrng, &base) == HAL_OK) {
        uint32_t index = base % WORDS_LENGTH;
        const char *word = wordle_dictionary[index];
        return word;
    }
    else {
        return "ratio";
    }
}

void game_main(flash_cfg_t* flashcfg, console_t* console) {
    h_flash_cfg = flashcfg;

    // initialise the morse processor
    morse_init(&h_morse);
    // initialise the  morsdle game engine
    morsdle_init_game(&h_game, morsdle_random_word());
    // plug the waveshare handle into the display container and then initialise
    canvas_init(&h_canvas, ROTATION_NINETY);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(&h_renderer, h_canvas.width, h_canvas.height);

    // allocate the 2-bit image buffer as we're using a waveshare display to render and the STM32L476 sucks for heap space
    if (imagebuffer_init(&h_image_buffer, h_canvas.width, h_canvas.height) != IMAGEBUFFER_OK) {
        assert(false);
    }

    h_display.init();

    // TODO : remove these calls eventually with something proper, but trying to align with the example code
    morse_action_event_t result = (morse_action_event_t) {};
    render_pass_t render_pass = (render_pass_t) {
            .display = &h_display,
            .canvas = &h_canvas,
    };
    morsdle_game_event_t ev;

    while (1) {
        uint32_t tick = HAL_GetTick();

        // keep calling this as it will eventually insert a delay so the next step can process
        bool inputConverted = morse_convert_input(&h_morse, tick);

        if(inputConverted) {
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            while (morse_decode(&h_morse, &result)) {
                if (result.type == MORSE_ACTION_ADD_LETTER) {
                    morsdle_add_letter(&h_game, result.ch);
                } else if (result.type == MORSE_ACTION_RESET_WORD) {
                    // if we are only rendering when there is a completed word then the reset
                    // should reset the whole word
                    if(h_renderer.game_mode == MORSDLE_GAME_WHOLE_WORD) {
                        morsdle_reset_word(&h_game);
                    }
                    // if we are rendering on each letter addition then just treat the reset
                    // as a backspace
                    else {
                        morsdle_remove_letter(&h_game);
                    }
                } else if (result.type == MORSE_ACTION_RESET_GAME) {
                    h_game.word = morsdle_random_word();
                    morsdle_clear(&h_game);
                } else {
                    // no-op!
                }
            }
        }

        // are there any game events that may require processing
        if (morsdle_has_events(&h_game)) {
            // we aren't going to bother drawing unless there's an event, and even then they may not dirty the display
            // so collect any dirty regions into a render_pass and then see if we actually need to redraw.
            render_pass_init(&render_pass);

            while (morsdle_has_events(&h_game)) {
                bool didRead = morsdle_read_event(&h_game, &ev);

                if (didRead) {
                    renderer_handle_event(&h_canvas, &h_renderer, &render_pass, &ev);
                }
            }

            render_pass_end(&render_pass);
        }

        uint64_t looptime = HAL_GetTick() - tick;

    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t tick = HAL_GetTick();

    // the circuit now contains a hardware debounce in the loop, so we can just assume that any
    // rising/falling edge we get is what we need to process.
    if(GPIO_Pin == MorseKey_Pin) {
        if (HAL_GPIO_ReadPin(MorseKey_GPIO_Port, MorseKey_Pin)) {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            morse_append_signal(&h_morse, SIGNAL_LOW, tick);
        } else {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

            morse_append_signal(&h_morse, SIGNAL_HIGH, tick);
        }
    }
    else if(GPIO_Pin == B1_Pin) {
        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)) {
            morse_append_signal(&h_morse, SIGNAL_LOW, tick);
        } else {
            morse_append_signal(&h_morse, SIGNAL_HIGH, tick);
        }

    }


}

