#include "unity.h"
#include "display.h"
#include "renderer.h"
#include "mock_display.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_gamecreatedevent() {
    canvas_t operations = &(struct canvas){};
    canvas_init(operations);

    renderer_t options = &(renderer) {};
    renderer_init(operations, options);

    morsdle_game_event_t ev = {
        .type = EVENT_GAME_CREATED
    };

    render_pass_t render_pass = (render_pass_t)&(struct render_pass){};
    render_pass_init(operations, render_pass);
    renderer_handle_event(operations, options, render_pass, &ev);
    render_pass_end(render_pass);

    mockdisplay_write_buffer(operations->display_impl, "test-rendergrid.svg");
    canvas_destroy(operations);
}

void test_wordcompletedevent() {
    canvas_t operations = &(struct canvas){};
    canvas_init(operations);

    renderer_t options = &(renderer) {};
    renderer_init(operations, options);

    morsdle_game_event_t ev = (morsdle_game_event_t){
            .type = EVENT_WORD_COMPLETED,
            .word = &(morsdle_word_t) {
                    .state = WORD_STATE_COMPLETE,
                    .letters = {
                            (morsdle_letter_t) {
                                    .state = LETTER_STATE_INVALID_LETTER,
                                    .x = 0,
                                    .y = 0
                            },
                            (morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID,
                                    .x = 1,
                                    .y = 0
                            },
                            (morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID_LETTER_INVALID_POSITION,
                                    .x = 2,
                                    .y = 0
                            },
                            (morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID,
                                    .x = 3,
                                    .y = 0
                            },
                            (morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID_LETTER_INVALID_POSITION,
                                    .x = 4,
                                    .y = 0
                            }
                    }
            }
    };

    render_pass_t render_pass = (render_pass_t)&(struct render_pass){};
    render_pass_init(operations, render_pass);
    renderer_handle_event(operations, options, render_pass, &ev);
    render_pass_end(render_pass);

    mockdisplay_write_buffer(operations->display_impl, "test-wordcompleted.svg");
    canvas_destroy(operations);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_gamecreatedevent);
    RUN_TEST(test_wordcompletedevent);
    return UNITY_END();
}