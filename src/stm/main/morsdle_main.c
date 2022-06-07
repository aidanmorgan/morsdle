#include <stdint.h>
#include "main.h"
#include "words.h"
#include "morse.h"
#include "morsdle.h"
#include "renderer.h"
#include "waveshare_display.h"
#include "waveshare_api.h"
#include "waveshare_spi.h"


static morse_t h_morse = (morse_t) { };
static morsdle_game_t h_game = (morsdle_game_t) { .word = "ratio" };

static renderer_t h_renderer = (renderer_t) {
        .game_mode = MORSDLE_GAME_SINGLE_LETTER
};

// this is the waveshare implementation of the handle
static display_impl_t display_impl = (display_impl_t) {
        .buffersize = WAVESHARE_BUFFER_SIZE,
        .render_dirty_region = waveshareapi_render_region,
//        .pre_render = waveshareapi_wake,
//        .post_render = waveshareapi_sleep,
        .pre_render = NULL,
        .post_render = NULL,
        .init = waveshareapi_init
};

static canvas_t h_canvas = (canvas_t) {
        .display_impl = &display_impl,
        .height = WAVESHARE_PIXEL_HEIGHT,
        .width = WAVESHARE_PIXEL_WIDTH
};

// passed into the init board function to get access to the static fields for use elsewhere in this
// main loop because the STM32CubeMX tool generates the handles to specific hardware as static instances
// to annoy me even when the "generate main" option is disabled.
static stm32_config_t hw_config;


static const char* morsdle_random_word() {
    uint32_t base = HAL_RNG_GetRandomNumber(hw_config.rng);
    uint32_t index = base % WORDS_LENGTH;
    const char* word = wordle_dictionary[index];
    return word;
}

int main(void) {
    init_stm_board(&hw_config);    // implemented by hand in the generated code to perform initialisation

    // i know i really should statically allocate this, but im trying to debug
    display_impl.buffer = (uint8_t*)malloc(sizeof(uint8_t)*WAVESHARE_BUFFER_SIZE);

    // initialise the morse processor
    morse_init(&h_morse);

    h_game.word = morsdle_random_word();
    // initialise the  morsdle game engine
    morsdle_init_game(&h_game);

    // plug the waveshare handle into the display container and then initialise
    canvas_init(&h_canvas);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(&h_canvas, &h_renderer);

    // initialise the waveshare display for a STM
    h_canvas.display_impl->init();

    while (1) {
        // this probably only needs to happen every 2 * MORSE_DIT ms (dit to high, dit to low)
        bool converted = morse_convert(&h_morse, 0);

        if (converted) {
            morse_action_event_t result = (morse_action_event_t){};
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            if (morse_decode(&h_morse, &result)) {
                if(result.type == MORSE_ACTION_ADD_LETTER) {
                    morsdle_add_letter(&h_game, result.ch);
                }
                else if(result.type == MORSE_ACTION_BACKSPACE) {
                    morsdle_remove_letter(&h_game);
                }
                else if(result.type == MORSE_ACTION_RESET) {
                    h_game.word = morsdle_random_word();
                    morsdle_clear(&h_game);
                }
                else {
                    // no-op!
                }
            }
        }

        // are there any game events that may require processing
        if (morsdle_has_events(&h_game)) {
            morsdle_game_event_t ev;

            // we aren't going to bother drawing unless there's an event, and even then they may not dirty the display
            // so collect any dirty regions into a render_pass and then see if we actually need to redraw.
            render_pass_t render_pass = (render_pass_t){};
            render_pass_init(&h_canvas, &render_pass);

//            while (morsdle_has_events(&h_game)) {
//                if (morsdle_read_event(&h_game, &ev)) {
//                    renderer_handle_event(&h_canvas, &h_renderer, &render_pass, &ev);
//                }
//            }
            waveshare_clear(&render_pass, COLOUR_RED);

            render_pass_end(&render_pass);
        }
    }

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // TODO : add some form of debouncing here
    if(GPIO_Pin == hw_config.button_pin) {
        if (HAL_GPIO_ReadPin(hw_config.button_port, hw_config.button_pin)) {
            morse_append_signal(&h_morse, SIGNAL_HIGH, HAL_GetTick());
        } else {
            morse_append_signal(&h_morse, SIGNAL_LOW, HAL_GetTick());
        }
    }
}

// currently configured to PC7
extern void wavesharespi_write_dc(uint8_t val) {
    HAL_GPIO_WritePin(hw_config.dc_port, hw_config.dc_pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// currently configured to PC9
extern void wavesharespi_write_cs(uint8_t val) {
    HAL_GPIO_WritePin(hw_config.cs_port, hw_config.cs_pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

// currently configured to PC8
extern void wavesharespi_write_rst(uint8_t val) {
    HAL_GPIO_WritePin(hw_config.rst_port, hw_config.rst_pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

// currently configured to SPI2
extern void wavesharespi_write_spi(uint8_t val) {
    HAL_SPI_Transmit(hw_config.spi_handle, &val, 1, 1000);
}

// currently configured to PC6
extern void wavesharespi_read_busy(uint8_t* val) {
    uint8_t result = HAL_GPIO_ReadPin(hw_config.busy_port, hw_config.busy_pin);
    *val = result;
}

extern void wavesharespi_delay(uint32_t time) {
    HAL_Delay(time);
}
