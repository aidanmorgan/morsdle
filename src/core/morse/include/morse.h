//
// Created by aidan on 22/04/2022.
//

#ifndef MORSE_H
#define MORSE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cbuff.h"

typedef enum {
    SIGNAL_VOID,
    SIGNAL_HIGH,
    SIGNAL_LOW
} signal_t;

typedef struct {
    signal_t value;
    uint64_t timestamp;
} morse_signal_t;

typedef enum {
    MORSE_NULL,
    MORSE_DOT,
    MORSE_DASH,
    MORSE_DELAY,
    MORSE_SHORT_HOLD,
    MORSE_LONG_HOLD
} morse_input_t;

typedef enum {
    MORSE_ACTION_NOOP,
    MORSE_ACTION_ADD_LETTER,
    MORSE_ACTION_BACKSPACE,
    MORSE_ACTION_RESET
} morse_action_t;

typedef struct {
    morse_action_t type;
    union {
        char ch;
    };
} morse_action_event_t;


// a dit is the "unit" of time of a morse code signal
#define MORSE_DIT_MS 75

// the minimum time in dits of the start of a "dot"
#define MORSE_DOT_START 0
// the minimum time in dits of the end of a "dot"
#define MORSE_DOT_END 3

// the maximum time in dits of the start of a "dash"
#define MORSE_DASH_START  3
// the maximum time in dits of the start of a "dash"
#define MORSE_DASH_END  7

// the minimum time in dits of when a "delay" starts
#define MORSE_DELAY_START 7

// start at 3 seconds
#define MORSE_SHORT_HOLD_START 40
// end at 5 seconds
#define MORSE_SHORT_HOLD_END 66

// start at 5 seconds
#define MORSE_LONG_HOLD_START 66
// end at 10 seconds
#define MORSE_LONG_HOLD_END 133

// if a signal change comes in within this time of the most recent value, then ignore it
// this is intentionally set to 1/5th of the MORSE_DIT_MS time
#define DEBOUNCE_THRESHOLD 15

struct morse {
    cbuff_t signal_buffer;
    cbuff_t morse_input_buffer;
};

typedef struct morse* morse_t;

void morse_init(morse_t morse);

bool morse_append_signal(morse_t morse, signal_t signal, uint64_t timestamp);

// is responsible for converting the buffer of SIGNAL_HIGH/SIGNAL_LOW into MORSE_DOT/MORSE_DASH/MORSE_DELAY/MORSE_SHORT_HOLD/MORSE_LONG_HOLD
bool morse_convert(morse_t morse, uint64_t timestamp);
// is responsible for converting the buffer of MORSE_DOT/MORSE_DASH/MORSE_DELAY in to a character
bool morse_decode(morse_t morse, morse_action_event_t* letter);






#endif //MORSE_H
