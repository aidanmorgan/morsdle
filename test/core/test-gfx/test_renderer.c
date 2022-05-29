#include "unity.h"
#include "display.h"
#include "renderer.h"
#include "mock_display.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_gamecreatedevent() {
    canvas_t canvas = &(struct canvas){};
    canvas_init(canvas);

    renderer_t options = &(renderer) {};
    renderer_init(canvas, options);

    morsdle_game_event_t ev = {
        .type = EVENT_GAME_CREATED
    };

    render_pass_t render_pass = (render_pass_t)&(struct render_pass){};
    render_pass_init(canvas, render_pass);
    renderer_handle_event(canvas, options, render_pass, &ev);

    TEST_ASSERT_EQUAL(14, render_pass->dirty_regions->size);
    region_t regions[14];
    cbuff_readmany(render_pass->dirty_regions, &regions, 14);

    // check that the first call is to clear the entire background, so the bounding box is EVERYTHING
    TEST_ASSERT_EQUAL(0, regions[0].top_left.x);
    TEST_ASSERT_EQUAL(0, regions[0].top_left.y);
    TEST_ASSERT_EQUAL(448, regions[0].bottom_right.x);
    TEST_ASSERT_EQUAL(600, regions[0].bottom_right.y);

    render_pass_end(render_pass);

    mockdisplay_write_buffer(canvas->display_impl, "test-rendergrid.svg");
    canvas_destroy(canvas);
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

    // this should have caused the 5 grid squares for each of the letters in the word on the first line to have changed
    TEST_ASSERT_EQUAL(5, render_pass->dirty_regions->size);
    region_t regions[5];
    cbuff_readmany(render_pass->dirty_regions, &regions, 5);

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