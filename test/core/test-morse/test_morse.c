#include <time.h>
#include "unity.h"
#include "cbuff.h"
#include "morse.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_morse_init() {
    struct morse val = (struct morse) {};
    morse_init(&val);

    TEST_ASSERT_NOT_NULL(val.signal_buffer);
}

void test_morse_append() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_HIGH, 500);

    TEST_ASSERT_EQUAL(1, cbuff_size(morseconfig->signal_buffer));
    morse_signal_t readback;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->signal_buffer, &readback, 1));

    TEST_ASSERT_EQUAL(500, readback.timestamp);
    TEST_ASSERT_EQUAL(SIGNAL_HIGH, readback.value);

    morse_append_signal(morseconfig, SIGNAL_LOW, 1000);

    TEST_ASSERT_EQUAL(2, cbuff_size(morseconfig->signal_buffer));
    morse_signal_t multipeek[2];
    TEST_ASSERT_EQUAL(2, cbuff_peektail(morseconfig->signal_buffer, &multipeek, 2));

    TEST_ASSERT_EQUAL(500, multipeek[0].timestamp);
    TEST_ASSERT_EQUAL(SIGNAL_HIGH, multipeek[0].value);
    TEST_ASSERT_EQUAL(1000, multipeek[1].timestamp);
    TEST_ASSERT_EQUAL(SIGNAL_LOW, multipeek[1].value);
}

void test_morse_process_dot() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_HIGH, 500);
    morse_append_signal(morseconfig, SIGNAL_LOW, 500 + (MORSE_DOT_START + 1 * MORSE_DIT_MS));

    morse_convert(morseconfig, 1500);

    TEST_ASSERT_EQUAL(2, cbuff_size(morseconfig->morse_input_buffer));
    morse_input_t val;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->morse_input_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DOT, val);

    // a delay should be automatically added through the processor if we run the process_signals and the timestamp
    // for the process is 7 * MORSE_DIT_MS after the last SIGNAL_LOW value
    TEST_ASSERT_EQUAL(1, cbuff_peekhead(morseconfig->morse_input_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DELAY, val);
}


void test_morse_process_dash() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_HIGH, 500);
    morse_append_signal(morseconfig, SIGNAL_LOW, 850);

    morse_convert(morseconfig, 2500);

    TEST_ASSERT_EQUAL(2, cbuff_size(morseconfig->morse_input_buffer));
    morse_input_t val;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->morse_input_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DASH, val);

    TEST_ASSERT_EQUAL(1, cbuff_peekhead(morseconfig->morse_input_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DELAY, val);
}

void test_morse_dotdotdash() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_LOW, 0);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 100);
    morse_append_signal(morseconfig, SIGNAL_LOW, 250);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 300);
    morse_append_signal(morseconfig, SIGNAL_LOW, 450);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 500);
    morse_append_signal(morseconfig, SIGNAL_LOW, 800);

    morse_convert(morseconfig, 5000);


    TEST_ASSERT_EQUAL(4, cbuff_size(morseconfig->morse_input_buffer));
    morse_input_t val[4];
    TEST_ASSERT_EQUAL(4, cbuff_peektail(morseconfig->morse_input_buffer, &val, 4));
    TEST_ASSERT_EQUAL(MORSE_DOT, val[0]);
    TEST_ASSERT_EQUAL(MORSE_DOT, val[1]);
    TEST_ASSERT_EQUAL(MORSE_DASH, val[2]);
    TEST_ASSERT_EQUAL(MORSE_DELAY, val[3]);
}

void test_morse_process_random() {
    struct morse* morseconfig = &(struct morse) {};

    // generate 1000 random sequences of DASH and DOT and use them to
    for(uint16_t i = 0; i < 10000; i++) {
        srand(time(NULL));
        // reset the buffer so we can loop again
        morse_init(morseconfig);

        // keep track of what we think we are creating so we can confirm the decoding later on
        morse_input_t expected[5];
        // rolling incrementing timestamp for the entries as they come in
        uint64_t timestamp = 0;

        for(uint8_t j = 0; j < 5; j++) {
            // we start with a low signal
            morse_append_signal(morseconfig, SIGNAL_HIGH, timestamp);

            double  dits_increment = 0;
            // create a random number between 0 and 1 that we can multiply out
            double random = (double)rand() / (double) RAND_MAX;

            if(rand() % 2 == 0) {
                expected[j] = MORSE_DOT;
                dits_increment = 1 + (random * ((MORSE_DOT_END - MORSE_DOT_START) - 1));
            }
            else {
                expected[j] = MORSE_DASH;
                dits_increment = 1 + MORSE_DASH_START + (random *  ((MORSE_DASH_END - MORSE_DASH_START - 1)));
            }

            // we want to offset the transition from HIGH->LOW by the duration
            // of a signal length, so multiply the number of dits out to get milliseconds
            timestamp += (uint64_t) (dits_increment * (double )MORSE_DIT_MS);

            morse_append_signal(morseconfig, SIGNAL_LOW, timestamp);

            // introduce a slight delay before we loop again
            timestamp += (rand() % 7) * MORSE_DIT_MS;
        }

        morse_convert(morseconfig, timestamp + (MORSE_DIT_MS * (MORSE_DELAY_START + 1)));

        TEST_ASSERT_EQUAL(6, cbuff_size(morseconfig->morse_input_buffer));

        morse_input_t processed[6];
        cbuff_readmany(morseconfig->morse_input_buffer, &processed, 6);

        for(uint8_t j = 0; j < 5; j++) {
            TEST_ASSERT_EQUAL(expected[j], processed[j]);
        }

        TEST_ASSERT_EQUAL(MORSE_DELAY, processed[5]);
    }
}

void test_morse_decode_shortsequence() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_input_t  input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('E', result.ch);
}

void test_morse_decode_longsequence() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_input_t input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('Z', result.ch);
    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_wholeword() {
    struct morse* morseconfig = &(struct morse) {};

    morse_init(morseconfig);
    morse_input_t input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

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
}

void test_morse_decode_invalidentry() {
    struct morse* morseconfig = &(struct morse) {};

    morse_init(morseconfig);
    morse_input_t input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t) {};
    TEST_ASSERT_FALSE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(0, cbuff_size(morseconfig->morse_input_buffer));
}

void test_morse_decode_overfullbuffer() {
    struct morse* morseconfig = &(struct morse) {};
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
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_LOW, 0);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 20);
    morse_append_signal(morseconfig, SIGNAL_LOW, 4000);

    morse_input_t input;
    TEST_ASSERT_TRUE(morse_convert(morseconfig, 5000));

    cbuff_peektail(morseconfig->morse_input_buffer, &input, 1);
    TEST_ASSERT_EQUAL(MORSE_SHORT_HOLD, input);

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_BACKSPACE, result.type);
}

void test_morse_long_hold() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_LOW, 0);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 20);
    morse_append_signal(morseconfig, SIGNAL_LOW, 9000);

    morse_input_t input;
    TEST_ASSERT_TRUE(morse_convert(morseconfig, 10000));

    cbuff_peektail(morseconfig->morse_input_buffer, &input, 1);
    TEST_ASSERT_EQUAL(MORSE_LONG_HOLD, input);

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_RESET, result.type);
}

void test_morse_letter_then_hold() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_LOW, 0);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 15);
    morse_append_signal(morseconfig, SIGNAL_LOW, 170);   // add a dot

    morse_append_signal(morseconfig, SIGNAL_HIGH, 200);
    morse_append_signal(morseconfig, SIGNAL_LOW, 350);  // add a dot

    // add a delay, then add a hold
    morse_append_signal(morseconfig, SIGNAL_HIGH, 2100);
    morse_append_signal(morseconfig, SIGNAL_LOW, 6100);

    TEST_ASSERT_TRUE(morse_convert(morseconfig, 7000));

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));

    TEST_ASSERT_EQUAL(MORSE_ACTION_ADD_LETTER, result.type);
    TEST_ASSERT_EQUAL('I', result.ch);

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(MORSE_ACTION_BACKSPACE, result.type);
}

void test_morse_letter_and_hold() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_append_signal(morseconfig, SIGNAL_LOW, 0);
    morse_append_signal(morseconfig, SIGNAL_HIGH, 1);
    morse_append_signal(morseconfig, SIGNAL_LOW, 151);   // add a dot

    morse_append_signal(morseconfig, SIGNAL_HIGH, 200);
    morse_append_signal(morseconfig, SIGNAL_LOW, 350);  // add a dot

    // add a delay, then add a hold
    morse_append_signal(morseconfig, SIGNAL_HIGH, 400);
    morse_append_signal(morseconfig, SIGNAL_LOW, 3500);

    TEST_ASSERT_TRUE(morse_convert(morseconfig, 4000));

    // so in this scenario, even though we have a matching letter (dot, dot) the hold
    // is intended to "override" the letter, so we should throw the buffer away
    morse_action_event_t result = (morse_action_event_t){};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL(MORSE_ACTION_BACKSPACE, result.type);

    TEST_ASSERT_FALSE(morse_decode(morseconfig, &result));
}

void test_morse_buffer_starts_with_delay() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_input_t input = MORSE_DELAY;

    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input= MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t){};

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('I', result.ch);

    // now lets try adding a heap of delays for fun
    cbuff_clear(morseconfig->morse_input_buffer);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('R', result.ch);
}

void test_morse_longest_letter() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig);

    morse_input_t input = MORSE_DOT;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DASH;
    cbuff_write(morseconfig->morse_input_buffer, &input);
    input = MORSE_DELAY;
    cbuff_write(morseconfig->morse_input_buffer, &input);

    morse_action_event_t result = (morse_action_event_t){};
    TEST_ASSERT_TRUE(morse_decode(morseconfig, &result));
    TEST_ASSERT_EQUAL('J', result.ch);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_morse_init);
    RUN_TEST(test_morse_append);
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