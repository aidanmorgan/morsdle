//
// Created by aidan on 22/04/2022.
//

#ifndef MORSE_H
#define MORSE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MORSE_DOT,
    MORSE_DASH
} morse_signal_t;

typedef struct {
    morse_signal_t signals[5];  // a morse character is at most 5 dot/dash long
    uint8_t length;
    bool dirty;
} morse_input_t;

// a dit is the "unit" of time of a morse code signal
#define MORSE_DIT_MS 500
#define MORSE_DOT_START MORSE_DIT_MS
#define MORSE_DOT_END (3 * MORSE_DIT_MS)
#define MORSE_DASH_START  (3 * MORSE_DIT_MS)
#define MORSE_DASH_END  (6 * MORSE_DIT_MS)

#define MORSE_RESET (7 * MORSE_DIT_MS)

static char morse_alphabet[26][5] = {
        {{MORSE_DOT, MORSE_DASH, NULL, NULL, NULL}, 'A'},
        {{MORSE_DASH, MORSE_DOT,MORSE_DOT,MORSE_DOT, NULL}, 'B'},

};

void morse_init();

// Returns TRUE if the timing period represented by the start and end tick is considered
// a valid morse signal input, setting the value of the morse_signal_t to the value, FALSE
// otherwise.
bool morse_signal_match(uint32_t start, uint32_t end, morse_signal_t* val);

// Returns TRUE if the sequence of morse_signal_t in the provided morse_input_t represents a morse
// code letter and sets the value of result to the character (always uppercase), FALSE otherwise.
bool morse_letter_match(morse_input_t* input, char* result);






#endif //MORSE_H
