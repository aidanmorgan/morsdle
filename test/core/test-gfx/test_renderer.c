#include "unity.h"
#include "display.h"
#include "renderer.h"
#include "mock_display.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_gamecreatedevent() {
    display_operations operations = (display_operations){};
    init_drawing_ops(&operations);

    renderer_options options = (renderer_options) {};
    init_renderer(&operations, &options);

    morsdle_game_event_t ev = {
        .type = EVENT_GAME_CREATED
    };

    renderer_handle_event(&operations, &options, &ev);

    mockdisplay_write_buffer(operations.handle, "test-rendergrid.svg");
    destroy_drawing_ops(&operations);
}

void test_wordcompletedevent() {
    display_operations operations = (display_operations){};
    init_drawing_ops(&operations);

    renderer_options options = (renderer_options) {};
    init_renderer(&operations, &options);

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

    // need to call clear to get the grid to render properly
    renderer_clear(&operations, &options);
    renderer_handle_event(&operations, &options, &ev);

    mockdisplay_write_buffer(operations.handle, "test-wordcompleted.svg");
    destroy_drawing_ops(&operations);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_gamecreatedevent);
    RUN_TEST(test_wordcompletedevent);
    return UNITY_END();
}