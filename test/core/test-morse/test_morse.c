#include <time.h>
#include "unity.h"
#include "cbuff.h"
#include "morse.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

#define MAX_UINT32 (0xFFFFFFFF)
#define HALF_UINT32_MAX (MAX_UINT32 / 2)

#define MORSE_DOT_START_MS (MORSE_DOT_START * MORSE_DIT_MS)
#define MORSE_DOT_END_MS (MORSE_DOT_END * MORSE_DIT_MS)
#define MORSE_DOT_RANGE_MS (MORSE_DOT_END_MS - MORSE_DOT_START_MS)

void append_dot(morse_t* morse, uint32_t* time) {
    uint32_t dot_time = 1 + MORSE_DOT_START_MS + (rand() % MORSE_DOT_RANGE_MS);

    uint32_t start = *time;

    morse_append_signal(morse, SIGNAL_HIGH, start);
    start += dot_time;
    morse_append_signal(morse, SIGNAL_LOW, start);

    *time = start;
}

#define MORSE_DASH_START_MS (MORSE_DASH_START * MORSE_DIT_MS)
#define MORSE_DASH_END_MS (MORSE_DASH_END * MORSE_DIT_MS)
#define MORSE_DASH_RANGE (MORSE_DASH_END - MORSE_DASH_START)

void append_dash(morse_t* morse, uint32_t* time) {
    uint32_t dash_time = 1 + MORSE_DASH_START_MS + (rand() % MORSE_DASH_RANGE);

    uint32_t start = *time;
    morse_append_signal(morse, SIGNAL_HIGH, start);
    start += dash_time;
    morse_append_signal(morse, SIGNAL_LOW, start);

    *time = start;
}

#define MORSE_DELAY_START_MS (MORSE_DELAY_START * MORSE_DIT_MS)
#define MORSE_DELAY_RANGE (10000)

void append_delay(uint32_t* time) {
    *time += MORSE_DELAY_START_MS + (rand() % MORSE_DELAY_RANGE);
}

#define PAUSE_START_MS (MORSE_DIT_MS)
#define PAUSE_END_MS ((MORSE_DELAY_START - 1) * MORSE_DIT_MS)
#define PAUSE_RANGE_MS (PAUSE_END_MS - PAUSE_START_MS)

void append_pause(uint32_t*time) {
    *time += PAUSE_START_MS + (rand() % PAUSE_RANGE_MS);
}

#define SHORT_HOLD_START_MS (MORSE_SHORT_HOLD_START * MORSE_DIT_MS)
#define SHORT_HOLD_END_MS (MORSE_SHORT_HOLD_END * MORSE_DIT_MS)
#define SHORT_HOLD_RANGE_MS (SHORT_HOLD_END_MS - SHORT_HOLD_START_MS)

void append_short_hold(morse_t*morse, uint32_t* time) {
    uint32_t hold_time = 1 + SHORT_HOLD_START_MS + (rand() % SHORT_HOLD_RANGE_MS);

    uint32_t start = *time;
    morse_append_signal(morse, SIGNAL_HIGH, start);
    start += hold_time;
    morse_append_signal(morse, SIGNAL_LOW, start);

    *time = start;
}

#define LONG_HOLD_START_MS (MORSE_LONG_HOLD_START * MORSE_DIT_MS)
#define LONG_HOLD_END_MS (MORSE_LONG_HOLD_END * MORSE_DIT_MS)
#define LONG_HOLD_RANGE_MS (LONG_HOLD_END_MS - LONG_HOLD_START_MS)

void append_long_hold(morse_t*morse, uint32_t* time) {
    uint32_t hold_time = 1 + LONG_HOLD_START_MS + (rand() % LONG_HOLD_RANGE_MS);

    uint32_t start = *time;
    morse_append_signal(morse, SIGNAL_HIGH, start);
    start += hold_time;
    morse_append_signal(morse, SIGNAL_LOW, start);

    *time = start;
}

void test_morse_init() {
    morse_t val = (morse_t) {};
    morse_init(&val);

    TEST_ASSERT_NOT_NULL(val.signal_buffer);
}

void test_morse_process_dot() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;
    append_dot(morseconfig, &timestamp);
    append_delay(&timestamp);

    morse_convert_input(morseconfig, timestamp);

    TEST_ASSERT_EQUAL(3, cbuff_size(morseconfig->morse_input_buffer));
    morse_input_t val[3];
    cbuff_readmany(morseconfig->morse_input_buffer, &val, 3);

    TEST_ASSERT_EQUAL(MORSE_DELAY, val[0]);
    TEST_ASSERT_EQUAL(MORSE_DOT, val[1]);
    TEST_ASSERT_EQUAL(MORSE_DELAY, val[2]);
}


void test_morse_process_dash() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);

    morse_convert_input(morseconfig, timestamp);
    morse_input_t val;

    TEST_ASSERT_EQUAL(3, cbuff_size(morseconfig->morse_input_buffer));

    TEST_ASSERT_EQUAL(1, cbuff_peektail_after(morseconfig->morse_input_buffer, &val, 0,1));
    TEST_ASSERT_EQUAL(MORSE_DELAY, val);

    TEST_ASSERT_EQUAL(1, cbuff_peektail_after(morseconfig->morse_input_buffer, &val, 1,1));
    TEST_ASSERT_EQUAL(MORSE_DASH, val);

    TEST_ASSERT_EQUAL(1, cbuff_peektail_after(morseconfig->morse_input_buffer, &val, 2, 1));
    TEST_ASSERT_EQUAL(MORSE_DELAY, val);
}



void test_morse_dotdotdash() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);

    morse_convert_input(morseconfig, timestamp);


    TEST_ASSERT_EQUAL(5, cbuff_size(morseconfig->morse_input_buffer));
    morse_input_t val[5];
    TEST_ASSERT_EQUAL(5, cbuff_peektail(morseconfig->morse_input_buffer, &val, 5));
    TEST_ASSERT_EQUAL(MORSE_DELAY, val[0]);
    TEST_ASSERT_EQUAL(MORSE_DOT, val[1]);
    TEST_ASSERT_EQUAL(MORSE_DOT, val[2]);
    TEST_ASSERT_EQUAL(MORSE_DASH, val[3]);
    TEST_ASSERT_EQUAL(MORSE_DELAY, val[4]);
}

void test_morse_process_random() {
    morse_t* morseconfig = &(morse_t) {};

    // generate 1000 random sequences of DASH and DOT and use them to
    for(uint16_t i = 0; i < 10000; i++) {
        // reset the buffer so we can loop again
        morse_init(morseconfig);

        // keep track of what we think we are creating so we can confirm the decoding later on
        morse_input_t expected[5];

        // rolling incrementing timestamp for the entries as they come in
        uint32_t timestamp = rand() % HALF_UINT32_MAX;

        for(uint8_t j = 0; j < 5; j++) {
            uint8_t x = rand() % 2;

            if(x == 0) {
                append_dot(morseconfig, &timestamp);
                expected[j] = MORSE_DOT;
            }
            else if(x == 1) {
                append_dash(morseconfig, &timestamp);
                expected[j] = MORSE_DASH;
            }

            append_pause(&timestamp);
        }

        append_delay(&timestamp);
        morse_convert_input(morseconfig, timestamp);

        TEST_ASSERT_EQUAL(7, cbuff_size(morseconfig->morse_input_buffer));

        morse_input_t processed[7];
        cbuff_readmany(morseconfig->morse_input_buffer, &processed, 7);

        TEST_ASSERT_EQUAL(MORSE_DELAY, processed[0]);

        for(uint8_t j = 1; j <= 5; j++) {
            TEST_ASSERT_EQUAL(expected[j], processed[j]);
        }

        TEST_ASSERT_EQUAL(MORSE_DELAY, processed[6]);
    }
}

void test_morse_decode_shortsequence() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t time = rand() % HALF_UINT32_MAX;

    append_dot(morseconfig, &time);
    append_delay(&time);

    morse_convert_input(morseconfig, time);
    morse_action_event_t result = (morse_action_event_t) {};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(MORSE_ACTION_ADD_LETTER, result.type);
    TEST_ASSERT_EQUAL('E', result.ch);

    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_longsequence() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_delay(&timestamp);

    // convert the signal values to dots and dashes
    morse_convert_input(morseconfig, timestamp);

    // now convert the dots and dashes to actions
    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('Z', result.ch);

    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_wholeword() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    // B -...
    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_delay(&timestamp);
    // U ..-
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);
    // T -
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);
    // T -
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);
    // S ...
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_delay(&timestamp);

    morse_convert_input(morseconfig, timestamp);

    morse_input_t expected[18] = {
        MORSE_DELAY,
        MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DELAY,   // b
        MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DELAY,              // u
        MORSE_DASH, MORSE_DELAY,                                    // t
        MORSE_DASH, MORSE_DELAY,                                    // t
        MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DELAY                // s
    };

    morse_input_t inputs [18];
    cbuff_peektail(morseconfig->morse_input_buffer, &inputs, 18);

    TEST_ASSERT_EQUAL_INT8_ARRAY(expected, inputs, 18);


    morse_action_event_t result = (morse_action_event_t) {};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('B', result.ch);
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('U', result.ch);
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('T', result.ch);
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('T', result.ch);
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('S', result.ch);

    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_invalidentry() {
    morse_t* morseconfig = &(morse_t) {};

    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);

    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_FALSE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_overfullbuffer() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);
    morse_input_t input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_FALSE(morse_decode(morseconfig, &result));
    // the maximum length has been reached, but we can't process anything, so the process
    // should by default throw away the oldest entry to try and process again
    TEST_ASSERT_EQUAL(5, cbuff_size(morseconfig->morse_input_buffer));

    // now the first dot has been removed we actually have a valid letter in the buffer, so this
    // should work on the next call correctly.
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('C', result.ch);
    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_short_hold() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;
    append_short_hold(morseconfig, &timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, timestamp));

    morse_input_t input[2];
    cbuff_peektail(morseconfig->morse_input_buffer, &input, 2);
    TEST_ASSERT_EQUAL(MORSE_DELAY, input[0]);
    TEST_ASSERT_EQUAL(MORSE_SHORT_HOLD, input[1]);

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_RESET_WORD, result.type);
}

void test_morse_long_hold() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;
    append_long_hold(morseconfig, &timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, 10000));

    morse_input_t input[2];
    cbuff_peektail(morseconfig->morse_input_buffer, &input, 2);
    TEST_ASSERT_EQUAL(MORSE_DELAY, input[0]);
    TEST_ASSERT_EQUAL(MORSE_LONG_HOLD, input[1]);

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_RESET_GAME, result.type);
}

void test_morse_letter_then_hold() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_delay(&timestamp);
    append_short_hold(morseconfig, &timestamp);
    append_delay(&timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, timestamp));

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_ADD_LETTER, result.type);
    TEST_ASSERT_EQUAL('I', result.ch);

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(MORSE_ACTION_RESET_WORD, result.type);
}

void test_morse_letter_and_hold() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    // adding a dot, dot and then a hold with no delay should cause the hold to override the first two entries
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_short_hold(morseconfig, &timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, timestamp));
    morse_input_t expected[4] = {
            MORSE_DELAY,
            MORSE_DOT,
            MORSE_DOT,
            MORSE_SHORT_HOLD
    };
    morse_input_t actual[4];
    cbuff_peektail(morseconfig->morse_input_buffer, &actual, 4);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, 4);

    // so in this scenario, even though we have a matching letter (dot, dot) the hold
    // is intended to "override" the letter, so we should throw the buffer away
    morse_action_event_t result = (morse_action_event_t){};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(MORSE_ACTION_RESET_WORD, result.type);

    TEST_ASSERT_FALSE(morse_decode(morseconfig, &result));
}

void test_morse_buffer_starts_with_delay() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    append_delay(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dot(morseconfig, &timestamp);
    append_delay(&timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, timestamp));
    morse_action_event_t result = (morse_action_event_t){};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('I', result.ch);
}

void test_morse_longest_letter() {
    morse_t* morseconfig = &(morse_t) {};
    morse_init(morseconfig);

    uint32_t timestamp = rand() % HALF_UINT32_MAX;

    append_dot(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_pause(&timestamp);
    append_dash(morseconfig, &timestamp);
    append_delay(&timestamp);

    TEST_ASSERT_TRUE(morse_convert_input(morseconfig, timestamp));

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('J', result.ch);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_morse_init);
    RUN_TEST(test_morse_process_dot);
    RUN_TEST(test_morse_process_dash);
    RUN_TEST(test_morse_dotdotdash);
    RUN_TEST(test_morse_process_random);
    RUN_TEST(test_morse_decode_shortsequence);
    RUN_TEST(test_morse_decode_longsequence);
    RUN_TEST(test_morse_decode_wholeword);
    RUN_TEST(test_morse_decode_invalidentry);
    RUN_TEST(test_morse_decode_overfullbuffer);

    RUN_TEST(test_morse_short_hold);
    RUN_TEST(test_morse_long_hold);
    RUN_TEST(test_morse_letter_then_hold);
    RUN_TEST(test_morse_letter_and_hold);
    RUN_TEST(test_morse_buffer_starts_with_delay);
    RUN_TEST(test_morse_longest_letter);

    return UNITY_END();
}