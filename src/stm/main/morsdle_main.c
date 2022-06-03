#include <stdint.h>
#include "main.h"
#include "morse.h"
#include "cbuff.h"
#include "morsdle.h"
#include "renderer.h"
#include "waveshare_display.h"
#include "waveshare_api.h"
#include "waveshare_spi.h"

static uint8_t imagebuffer[448 * 600];

static morse_t h_morse = &(struct morse) { };
static morsdle_game_t *h_game = &(morsdle_game_t) { .word = "ratio" };

static renderer_t h_renderer = &(renderer) {
        .game_mode = MORSDLE_GAME_SINGLE_LETTER
};

// this is the waveshare implementation of the handle
static display_impl_t display_impl = (display_impl_t) {
        .buffer = imagebuffer,
        .render_dirty_region = waveshareapi_render_region,
        .pre_render = waveshareapi_wake,
        .post_render = waveshareapi_sleep,
        .init = waveshareapi_init
};

static canvas_t h_canvas = &(struct canvas) {
        .display_impl = &display_impl,
        .height = 600,
        .width = 448
};



int main(void) {
    InitStm32L4xx();    // implemented by hand in the generated code to perform initialisation

    // initialise the morse processor
    morse_init(h_morse);

    // initialise the  morsdle game engine
    morsdle_init_game(h_game);

    // plug the waveshare handle into the display container and then initialise
    canvas_init(h_canvas);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(h_canvas, h_renderer);

    // initialise the waveshare display for a STM
    h_canvas->display_impl->init();

    while (1) {
        // this probably only needs to happen every 2 * MORSE_DIT ms (dit to high, dit to low)
        bool converted = morse_convert(h_morse, 0);

        if (converted) {
            morse_action_event_t result = (morse_action_event_t){};
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            if (morse_decode(h_morse, &result)) {
                if(result.type == MORSE_ACTION_ADD_LETTER) {
                    morsdle_add_letter(h_game, result.ch);
                }
                else if(result.type == MORSE_ACTION_BACKSPACE) {
                    morsdle_remove_letter(h_game);
                }
                else if(result.type == MORSE_ACTION_RESET) {
                    // TODO : implement me
                }
                else {
                    // no-op!
                }
            }
        }

        // are there any game events that may require processing
        if (morsdle_has_events(h_game)) {
            morsdle_game_event_t ev;

            // we aren't going to bother drawing unless there's an event, and even then they may not dirty the display
            // so collect any dirty regions into a render_pass and then see if we actually need to redraw.
            struct render_pass render_pass;
            render_pass_init(h_canvas, &render_pass);

            while (morsdle_has_events(h_game)) {
                if (morsdle_read_event(h_game, &ev)) {
                    renderer_handle_event(h_canvas, h_renderer, &render_pass, &ev);
                }
            }

            render_pass_end(&render_pass);
        }
    }

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // TODO : add some form of debouncing here
    if(GPIO_Pin == B1_Pin) {
        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)) {
            morse_append_signal(h_morse, SIGNAL_HIGH, HAL_GetTick());
        } else {
            morse_append_signal(h_morse, SIGNAL_LOW, HAL_GetTick());
        }
    }
}

extern void wavesharespi_write_dc(uint8_t val) {
    HAL_GPIO_WritePin(Waveshare_Dc_GPIO_Port, Waveshare_Dc_Pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

extern void wavesharespi_write_cs(uint8_t val) {
    HAL_GPIO_WritePin(Waveshare_Cs_GPIO_Port, Waveshare_Cs_Pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

extern void wavesharespi_write_rst(uint8_t val) {
    HAL_GPIO_WritePin(Waveshare_Rst_GPIO_Port, Waveshare_Rst_Pin, val == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

extern void wavesharespi_write_spi(uint8_t val) {
    HAL_SPI_Transmit(&hspi2, &val, 1, 1000);
}

extern void wavesharespi_read_busy(uint8_t* val) {
    uint8_t result = HAL_GPIO_ReadPin(Waveshare_Busy_GPIO_Port, Waveshare_Busy_Pin);
    *val = result;
}

extern void wavesharespi_delay(uint32_t time) {
    HAL_Delay(time);
}
