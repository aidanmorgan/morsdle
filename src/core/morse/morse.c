#include "morse.h"

#define MAX_INPUTS_PER_LETTER 5

typedef struct {
    uint8_t letter;
    morse_input_t values[MAX_INPUTS_PER_LETTER];
} morse_table_entry_t;

// This lookup table is based on (what I believe) the fact that the most common occuring letters are shorter in sequence
// so if we group all of the letters of the same morse length together we can have a shorter list to loop through when
// we detect a delay, this way we just look at the length of the sequence, get the array of potential matches, loop throgh
// them until a NULL is encountered or we get a comparison match.
//
// Doubled with the fact that the list is always 5 entries long, even if the tail end are NULL we should get some speed of
// alignment/consistency benefits by just at worst looking through 12 entries rather than what was there originally which required always
// scanning thouhgh all 26 letters.
//
// My theory is (to be tested on a device, I am in Donnelley River right now with no internet access) that because the morse
// table encodes more frequent letters with less dits, and there are less of them (due to the reduced encoding space of shorter
// sequences) I should be able to get more performance here in the lookups - for example, finding the letter E would require at MOST
// 2 memcmp's given this design, wheres it would have originally taken 5, and for the letter T, it is at MOST 2, whereas in the original
// design it would have been 15 memcmp's which are relatively computationally expensive operations.
const static morse_table_entry_t* morse_lookup_table[4][12] = {
    // letters with an input length of 2 (including the DELAY)
    {
        &(morse_table_entry_t) { .letter = 'E', .values ={MORSE_DOT,  MORSE_DELAY, MORSE_NULL,  MORSE_NULL,  MORSE_NULL}}, // e
        &(morse_table_entry_t) { .letter = 'T', .values ={MORSE_DASH, MORSE_DELAY, MORSE_NULL,  MORSE_NULL,  MORSE_NULL}}, // t
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
    },
    // letters with an input length of 3 (including the DELAY)
    {
        &(morse_table_entry_t) { .letter = 'A', .values = {MORSE_DOT,  MORSE_DASH,  MORSE_DELAY, MORSE_NULL,  MORSE_NULL}}, // a
        &(morse_table_entry_t) { .letter = 'I', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DELAY, MORSE_NULL,  MORSE_NULL}}, // i
        &(morse_table_entry_t) { .letter = 'M', .values ={MORSE_DASH, MORSE_DASH,  MORSE_DELAY, MORSE_NULL,  MORSE_NULL}}, // m
        &(morse_table_entry_t) { .letter = 'N', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DELAY, MORSE_NULL,  MORSE_NULL}}, // n
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
    },
    // letters with an input length of 4 (including the DELAY)
    {
        &(morse_table_entry_t) { .letter = 'D', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DOT,   MORSE_DELAY, MORSE_NULL}}, // d
        &(morse_table_entry_t) { .letter = 'G', .values ={MORSE_DASH, MORSE_DASH,  MORSE_DOT,   MORSE_DELAY, MORSE_NULL}}, // g
        &(morse_table_entry_t) { .letter = 'K', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DASH,  MORSE_DELAY, MORSE_NULL}}, // k
        &(morse_table_entry_t) { .letter = 'O', .values ={MORSE_DASH, MORSE_DASH,  MORSE_DASH,  MORSE_DELAY, MORSE_NULL}}, // o
        &(morse_table_entry_t) { .letter = 'R', .values ={MORSE_DOT,  MORSE_DASH,  MORSE_DOT,   MORSE_DELAY, MORSE_NULL}}, // r
        &(morse_table_entry_t) { .letter = 'S', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DOT,   MORSE_DELAY, MORSE_NULL}}, // s
        &(morse_table_entry_t) { .letter = 'U', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DASH,  MORSE_DELAY, MORSE_NULL}}, // u
        &(morse_table_entry_t) { .letter = 'W', .values ={MORSE_DOT,  MORSE_DASH,  MORSE_DASH,  MORSE_DELAY, MORSE_NULL}}, // w
        NULL,NULL,NULL,NULL
    },
    // letters with an input length of 5 (including the DELAY)
    // TODO : Resort the list below based on a frequency analysis of the wordle dictionary so the more frequent letters appear earlier
    {
        &(morse_table_entry_t) { .letter = 'B', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DOT,   MORSE_DOT,   MORSE_DELAY}}, // b
        &(morse_table_entry_t) { .letter = 'C', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DASH,  MORSE_DOT,   MORSE_DELAY}}, // c
        &(morse_table_entry_t) { .letter = 'F', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DASH,  MORSE_DOT,   MORSE_DELAY}}, // f
        &(morse_table_entry_t) { .letter = 'H', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DOT,   MORSE_DOT,   MORSE_DELAY}}, // h
        &(morse_table_entry_t) { .letter = 'J', .values ={MORSE_DOT,  MORSE_DASH,  MORSE_DASH,  MORSE_DASH,  MORSE_DELAY}}, // j
        &(morse_table_entry_t) { .letter = 'L', .values ={MORSE_DOT,  MORSE_DASH,  MORSE_DOT,   MORSE_DOT,   MORSE_DELAY}}, // l
        &(morse_table_entry_t) { .letter = 'P', .values ={MORSE_DOT,  MORSE_DASH,  MORSE_DASH,  MORSE_DOT,   MORSE_DELAY}}, // p
        &(morse_table_entry_t) { .letter = 'Q', .values ={MORSE_DASH, MORSE_DASH,  MORSE_DOT,   MORSE_DASH,  MORSE_DELAY}}, // q
        &(morse_table_entry_t) { .letter = 'V', .values ={MORSE_DOT,  MORSE_DOT,   MORSE_DOT,   MORSE_DASH,  MORSE_DELAY}}, // v
        &(morse_table_entry_t) { .letter = 'X', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DOT,   MORSE_DASH,  MORSE_DELAY}}, // x
        &(morse_table_entry_t) { .letter = 'Y', .values ={MORSE_DASH, MORSE_DOT,   MORSE_DASH,  MORSE_DASH,  MORSE_DELAY}}, // y
        &(morse_table_entry_t) { .letter = 'Z', .values ={MORSE_DASH, MORSE_DASH,  MORSE_DOT,   MORSE_DOT,   MORSE_DELAY}}  // z
    }
};

static morse_signal_t signal_backing_buffer[64];
static cbuff_t signal_buffer = (cbuff_t) {};

static morse_input_t action_backing_buffer[32];
static cbuff_t action_buffer = (cbuff_t) {};

void morse_init(morse_t *morse) {
    cbuff_init(&signal_buffer, &signal_backing_buffer, 64, sizeof(morse_signal_t));
    cbuff_init(&action_buffer, &action_backing_buffer, 32, sizeof(morse_input_t));

    morse->signal_buffer = &signal_buffer;
    morse->morse_input_buffer = &action_buffer;

    // initialise the buffer to be initialised to having a low signal
    morse_signal_t sig = (morse_signal_t) {
        .value = SIGNAL_LOW,
        .timestamp = 0
    };
    cbuff_write(&signal_buffer, &sig);
}

bool morse_append_signal(morse_t *morse, signal_t signal, uint32_t timestamp) {
    if (signal == SIGNAL_VOID) {
        return false;
    }

    // don't bother doing this on an empty buffer
    if(cbuff_size(morse->signal_buffer) > 0) {
        morse_signal_t head;
        cbuff_peekhead(morse->signal_buffer, &head, 1);

        if (signal == head.value) {
            return false;
        }

        if (!cbuff_canwrite(morse->signal_buffer)) {
            return false;
        }
    }

    morse_signal_t entry = (morse_signal_t) {
            .value = signal,
            .timestamp = timestamp
    };

    cbuff_write(morse->signal_buffer, &entry);
    return true;
}

bool morse_convert_input(morse_t *morse, uint32_t timestamp) {
    bool processing_performed = false;
    size_t signal_buffer_size = cbuff_size(morse->signal_buffer);

    size_t processed_idx = 0;

    if (signal_buffer_size >= 1) {
        morse_signal_t signals[signal_buffer_size];
        size_t readcount = cbuff_peektail(morse->signal_buffer, signals, signal_buffer_size);


        for (size_t i = 1; i < readcount; i++) {
            morse_signal_t prev = signals[i - 1];
            morse_signal_t current = signals[i];

            // find the difference in milliseconds from the last transition to the current one
            // and then convert that to "dits" which are the basic unit of how morse code
            // is determined...
            uint32_t duration = current.timestamp - prev.timestamp;

            double dits = (double) duration / (double) MORSE_DIT_MS;

            morse_input_t input = MORSE_NULL;

            // we want to detect a transition from high to low, the timestamp records the
            // time that the transition occurred, so we can subtract
            if (prev.value == SIGNAL_HIGH && current.value == SIGNAL_LOW) {
                // TODO : this should probably be changed to be an "adaptive" value rather than
                // TODO : a fixed value, but *shrug*

                if (dits > MORSE_DOT_START && dits <= MORSE_DOT_END) {
                    input = MORSE_DOT;
                } else if (dits > MORSE_DOT_END && dits <= MORSE_DASH_END) {
                    input = MORSE_DASH;
                } else if (dits > MORSE_SHORT_HOLD_START && dits <= MORSE_SHORT_HOLD_END) {
                    input = MORSE_SHORT_HOLD;
                } else if (dits > MORSE_LONG_HOLD_START && dits <= MORSE_LONG_HOLD_END) {
                    input = MORSE_LONG_HOLD;
                }

                cbuff_write(morse->morse_input_buffer, &input);
                processed_idx = i;
            }
            // if we are transitioning from LOW to HIGH, check and see if a delay should be added to the buffer before
            // we process the next transition.
            else if(prev.value == SIGNAL_LOW && current.value == SIGNAL_HIGH) {

                morse_input_t headInput;
                cbuff_peekhead(morse->morse_input_buffer, &headInput, 1);

                if(dits > MORSE_DELAY_START && headInput != MORSE_DELAY) {
                    morse_input_t delay = MORSE_DELAY;
                    cbuff_write(morse->morse_input_buffer, &delay);
                }
            }
        }

        if (processed_idx > 0) {
            cbuff_seek(morse->signal_buffer, processed_idx);
            processing_performed = true;
        }

        // we have a signal in the buffer, it's pretty likely to be a LOW, so we need to check the time and see if
        // we should append a delay to the input queue
        morse_signal_t headSignal = signals[readcount - 1];
        double headDifferenceDits = ((double)(timestamp - headSignal.timestamp)) / (double)MORSE_DIT_MS;

        if(headDifferenceDits > MORSE_DELAY_START && headSignal.value == SIGNAL_LOW) {
            morse_input_t headInput;
            cbuff_peekhead(morse->morse_input_buffer, &headInput, 1);

            // don't write a delay to the input buffer if there is already one there...
            if (headInput != MORSE_DELAY) {
                headInput = MORSE_DELAY;
                cbuff_write(morse->morse_input_buffer, &headInput);

                processing_performed = true;
            }
        }
    }

    return processing_performed;
}

static inline uint8_t min(uint8_t a, uint8_t b) {
    return a < b ? a : b;
}

bool morse_decode(morse_t *morse, morse_action_event_t *letter) {
    // default this to a NO-OP unless it's set to something else
    letter->type = MORSE_ACTION_NOOP;

    size_t buffer_length = cbuff_size(morse->morse_input_buffer);

    if (buffer_length == 0) {
        return false;
    }

    uint8_t offset = 0;
    morse_input_t temp;

    // we need to trim off the front of the list until there is something actionable at the front of the list
    cbuff_peektail_after(morse->morse_input_buffer, &temp, offset, 1);
    while ((temp == MORSE_DELAY || temp == MORSE_NULL) && offset < buffer_length) {
        offset++;
        cbuff_peektail_after(morse->morse_input_buffer, &temp, offset, 1);
    }

    if ((int8_t) (buffer_length - offset) <= 0) {
        return false;
    }

    // now we have something actionable at the front of the queue, so we can start processing the entries
    morse_input_t inputs[MAX_INPUTS_PER_LETTER];

    for (uint8_t i = 0; i < min((buffer_length - offset), MAX_INPUTS_PER_LETTER); i++) {
        // make sure we initialise the temporary array to all MORSE_NULL so the memory compare works properly later on
        memset(&inputs, MORSE_NULL, MAX_INPUTS_PER_LETTER * sizeof(morse_input_t));
        cbuff_peektail_after(morse->morse_input_buffer, &inputs, offset, i + 1);

        // we check for the HOLD before the DELAY as we want the input of the HOLD to take priority over
        // processing any characters, that is, if the user has partially keyed a letter and then realises
        // they have made a mistake and want to backspace we will clear their input up to the hold and
        // take that action - effectively throwing away the partially completed letter
        if (inputs[i] == MORSE_SHORT_HOLD) {
            letter->type = MORSE_ACTION_RESET_WORD;
            cbuff_seek(morse->morse_input_buffer, offset + i + 1);

            return true;
        } else if (inputs[i] == MORSE_LONG_HOLD) {
            letter->type = MORSE_ACTION_RESET_GAME;
            cbuff_seek(morse->morse_input_buffer, offset + i + 1);

            return true;
        }
        else if (i >= 1 && (inputs[i] == MORSE_DELAY)) {
            // see the relatively long essay of a comment at the top of this file to explain what
            // this code is actually doing as it's not immediately obvious, but THEORETICALLY should be
            // fairly performant
            const morse_table_entry_t** length_table = morse_lookup_table[i - 1];

            uint8_t idx = 0;
            while(length_table[idx] != NULL) {
                const morse_table_entry_t * entry = length_table[idx];

                if(memcmp(&(inputs[0]), entry->values, MAX_INPUTS_PER_LETTER * sizeof(morse_input_t)) == 0) {
                    letter->type = MORSE_ACTION_ADD_LETTER;
                    letter->ch = entry->letter;

                    // we have found a match, so we want to move the read pointer forward through
                    // the buffer so they aren't ingested in the future.
                    cbuff_seek(morse->morse_input_buffer, offset + i + 1);
                    return true;
                }

                idx++;
            }

            // if we get to this point, we've found a delay which means we want to try and match against
            // the letters, but it doesnt match anything in the morse table, but we need to reject the
            // entries and move through the buffer, otherwise we will just spin at this point forever.
            cbuff_seek(morse->morse_input_buffer, offset + i + 1);
            return false;
        }

    }

    // if we have hit the maximum size and we haven't found a delay, then throw away the head of the
    // buffer as there is no way we will ever move it otherwise and there's maybe a delay after that
    if (buffer_length >= MAX_INPUTS_PER_LETTER) {
        cbuff_seek(morse->morse_input_buffer, 1);
    }

    return false;
}

