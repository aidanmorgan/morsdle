#include "unity.h"
#include "display.h"
#include "renderer.h"
#include "mock_display.h"

#define  MOCK_DISPLAY_WIDTH 448
#define MOCK_DISPLAY_HEIGHT 600

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */


void test_gamecreatedevent(uint16_t width, uint16_t height, char* filename) {
    display_impl_t* display = (display_impl_t*)INLINE_MALLOC(display_impl_t, .buffer = svg_create(width, height), .width = width, .height = height);

    canvas_t* canvas = &(canvas_t){ };
    canvas_init(canvas);

    renderer_t* renderer = &(renderer_t) {};
    renderer_init(renderer, width, height);

    morsdle_game_event_t ev = {
        .type = EVENT_GAME_CREATED
    };

    render_pass_t* render_pass = &(render_pass_t){
        .canvas = canvas,
        .display = display
    };
    render_pass_init(render_pass);
    renderer_handle_event(canvas, renderer, render_pass, &ev);

    TEST_ASSERT_EQUAL(14, render_pass->dirty_regions->size);
    rectangle_t regions[14];
    cbuff_readmany(render_pass->dirty_regions, &regions, 14);

    TEST_ASSERT_EQUAL(0, regions[0].top_left.x);
    TEST_ASSERT_EQUAL(0, regions[0].top_left.y);
    TEST_ASSERT_EQUAL(MOCK_DISPLAY_WIDTH, regions[0].bottom_right.x);
    TEST_ASSERT_EQUAL(MOCK_DISPLAY_HEIGHT, regions[0].bottom_right.y);

    render_pass_end(render_pass);

    mockdisplay_write_buffer(display, filename);
    canvas_destroy(canvas);

    free(display);
}

void test_gamecreatedevent_vertical() {
    test_gamecreatedevent(448, 600, "test-gamecreated-vertical.svg");
}

void test_gamecreatedevent_horizontal() {
    test_gamecreatedevent(600, 448, "test-gamecreated-horizontal.svg");

}

void test_wordcompletedevent(uint16_t width, uint16_t height, char* filename) {
    display_impl_t* display = (display_impl_t*)INLINE_MALLOC(display_impl_t, .buffer = svg_create(width, height), .width = width, .height = height);

    canvas_t* operations = &(canvas_t){};
    canvas_init(operations);

    renderer_t* options = &(renderer_t) {};
    renderer_init(options, width, height);

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

    mockdisplay_write_buffer(display, filename);
    canvas_destroy(operations);

    free(display);
}

void test_wordcompletedevent_vertical() {
    test_wordcompletedevent(448, 600, "test-wordcompletedevent-vertical.svg");
}

void test_wordcompletedevent_horizontal() {
    test_wordcompletedevent(600, 448, "test-wordcompletedevent-horizontal.svg");
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_gamecreatedevent_vertical);
    RUN_TEST(test_gamecreatedevent_horizontal);
    RUN_TEST(test_wordcompletedevent_vertical);
    RUN_TEST(test_wordcompletedevent_horizontal);
    return UNITY_END();
}