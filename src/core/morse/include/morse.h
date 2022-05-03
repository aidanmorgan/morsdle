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
    MORSE_DASH
} morse_input_t;


// a dit is the "unit" of time of a morse code signal
#define MORSE_DIT_MS 500
#define MORSE_DOT_START MORSE_DIT_MS
#define MORSE_DOT_END (3 * MORSE_DIT_MS)
#define MORSE_DASH_START  (3 * MORSE_DIT_MS)
#define MORSE_DASH_END  (6 * MORSE_DIT_MS)

#define MORSE_RESET (7 * MORSE_DIT_MS)

struct morse {
    cbuff_t signal_buffer;
    cbuff_t morse_buffer;
    // called by the morse processor to append a signal to the buffer in the provided
    // handle, returning TRUE if a signal was appended, FALSE otherwise
    bool (*parse_callback)(struct morse* handle, char* character);
};

typedef struct morse* morse_t;

void morse_init(morse_t morse, bool (*callback)(morse_t morse, char*ch));

bool morse_append(morse_t morse, morse_signal_t signal);

bool morse_process_signals(morse_t morse);






#endif //MORSE_H
