#include "unity.h"
#include "display.h"
#include "renderer.h"
#include "mock_display.h"

#define  MOCK_DISPLAY_WIDTH 448
#define MOCK_DISPLAY_HEIGHT 600

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_gamecreatedevent() {
    display_impl_t* display = (display_impl_t*)INLINE_MALLOC(display_impl_t, .buffer = svg_create(MOCK_DISPLAY_WIDTH, MOCK_DISPLAY_HEIGHT), .width = MOCK_DISPLAY_WIDTH, .height = MOCK_DISPLAY_HEIGHT);

    canvas_t* canvas = &(canvas_t){ };
    canvas_init(canvas);

    renderer_t* options = &(renderer_t) {};
    renderer_init(canvas, options);

    morsdle_game_event_t ev = {
        .type = EVENT_GAME_CREATED
    };

    render_pass_t* render_pass = &(render_pass_t){
        .canvas = canvas,
        .display = display
    };
    render_pass_init(render_pass);
    renderer_handle_event(canvas, options, render_pass, &ev);

    TEST_ASSERT_EQUAL(13, render_pass->dirty_regions->size);
    rectangle_t regions[13];
    cbuff_readmany(render_pass->dirty_regions, &regions, 13);
    render_pass_end(render_pass);

    mockdisplay_write_buffer(display, "test-rendergrid.svg");
    canvas_destroy(canvas);

    free(display);
}

void test_wordcompletedevent() {
    display_impl_t* display = (display_impl_t*)INLINE_MALLOC(display_impl_t, .buffer = svg_create(MOCK_DISPLAY_WIDTH, MOCK_DISPLAY_HEIGHT), .width = MOCK_DISPLAY_WIDTH, .height = MOCK_DISPLAY_HEIGHT);

    canvas_t* operations = &(canvas_t){};
    canvas_init(operations);

    renderer_t* options = &(renderer_t) {};
    renderer_init(operations, options);

    morsdle_game_event_t ev = (morsdle_game_event_t){
            .type = EVENT_WORD_COMPLETED,
            .word = &(morsdle_word_t) {
                    .state = WORD_STATE_COMPLETE,
                    .letters = {
                            &(morsdle_letter_t) {
                                    .state = LETTER_STATE_INVALID_LETTER,
                                    .x = 0,
                                    .y = 0
                            },
                            &(morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID,
                                    .x = 1,
                                    .y = 0
                            },
                            &(morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID_LETTER_INVALID_POSITION,
                                    .x = 2,
                                    .y = 0
                            },
                            &(morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID,
                                    .x = 3,
                                    .y = 0
                            },
                            &(morsdle_letter_t) {
                                    .state = LETTER_STATE_VALID_LETTER_INVALID_POSITION,
                                    .x = 4,
                                    .y = 0
                            }
                    }
            }
    };

    render_pass_t* render_pass = &(render_pass_t){
        .display = display,
        .canvas = operations
    };
    render_pass_init(render_pass);
    renderer_handle_event(operations, options, render_pass, &ev);

    // this should have caused the 5 grid squares for each of the letters in the word on the first line to have changed
    TEST_ASSERT_EQUAL(5, render_pass->dirty_regions->size);
    rectangle_t regions[5];
    cbuff_readmany(render_pass->dirty_regions, &regions, 5);

    render_pass_end(render_pass);

    mockdisplay_write_buffer(display, "test-wordcompleted.svg");
    canvas_destroy(operations);

    free(display);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_gamecreatedevent);
    RUN_TEST(test_wordcompletedevent);
    return UNITY_END();
}