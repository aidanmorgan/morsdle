#include "unity.h"
#include "cbuff.h"
#include "morse.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_morse_init() {
    struct morse val = (struct morse) {};
    morse_init(&val, NULL);

    TEST_ASSERT_NOT_NULL(val.signal_buffer);
}

void test_morse_append() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig, NULL);

    morse_append(morseconfig, (morse_signal_t) {
        .timestamp = 500,
        .value = SIGNAL_HIGH
    });

    TEST_ASSERT_EQUAL(1, cbuff_size(morseconfig->signal_buffer));
    morse_signal_t readback;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->signal_buffer, &readback, 1));

    TEST_ASSERT_EQUAL(500, readback.timestamp);
    TEST_ASSERT_EQUAL(SIGNAL_HIGH, readback.value);

    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 1000,
            .value = SIGNAL_LOW
    });

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
    morse_init(morseconfig, NULL);

    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 500,
            .value = SIGNAL_HIGH
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 1500,
            .value = SIGNAL_LOW
    });

    morse_process_signals(morseconfig);

    TEST_ASSERT_EQUAL(1, cbuff_size(morseconfig->morse_buffer));
    morse_input_t val;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->morse_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DOT, val);
}


void test_morse_process_dash() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig, NULL);

    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 500,
            .value = SIGNAL_HIGH
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 2500,
            .value = SIGNAL_LOW
    });

    morse_process_signals(morseconfig);

    TEST_ASSERT_EQUAL(1, cbuff_size(morseconfig->morse_buffer));
    morse_input_t val;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(morseconfig->morse_buffer, &val, 1));
    TEST_ASSERT_EQUAL(MORSE_DASH, val);
}

void test_morse_dotdotdash() {
    struct morse* morseconfig = &(struct morse) {};
    morse_init(morseconfig, NULL);

    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 0,
            .value = SIGNAL_LOW
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 500,
            .value = SIGNAL_HIGH
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 1000,
            .value = SIGNAL_LOW
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 1500,
            .value = SIGNAL_HIGH
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 2000,
            .value = SIGNAL_LOW
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 2500,
            .value = SIGNAL_HIGH
    });
    morse_append(morseconfig, (morse_signal_t) {
            .timestamp = 5000,
            .value = SIGNAL_LOW
    });

    morse_process_signals(morseconfig);

    TEST_ASSERT_EQUAL(3, cbuff_size(morseconfig->morse_buffer));
    morse_input_t val[3];
    TEST_ASSERT_EQUAL(3, cbuff_peektail(morseconfig->morse_buffer, &val, 3));
    TEST_ASSERT_EQUAL(MORSE_DOT, val[0]);
    TEST_ASSERT_EQUAL(MORSE_DOT, val[1]);
    TEST_ASSERT_EQUAL(MORSE_DASH, val[2]);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_morse_init);
    RUN_TEST(test_morse_append);
    RUN_TEST(test_morse_process_dot);
    RUN_TEST(test_morse_process_dash);
    RUN_TEST(test_morse_dotdotdash);

    return UNITY_END();
}