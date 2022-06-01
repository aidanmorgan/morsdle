#include "morse.h"

const static morse_input_t morse_table[26][5] = {
        {MORSE_DOT, MORSE_DASH, MORSE_DELAY, MORSE_NULL, MORSE_NULL}, // a
        {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DELAY}, // b
        {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_DELAY}, // c
        {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DELAY, MORSE_NULL}, // d
        {MORSE_DOT, MORSE_DELAY, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // e
        {MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_DELAY}, // f
        {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DELAY, MORSE_NULL}, // g
        {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DELAY}, // h
        {MORSE_DOT, MORSE_DOT, MORSE_DELAY, MORSE_NULL, MORSE_NULL}, // i
        {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DELAY}, // j
        {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DELAY, MORSE_NULL}, // k
        {MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DELAY}, // l
        {MORSE_DASH, MORSE_DASH, MORSE_DELAY, MORSE_NULL, MORSE_NULL}, // m
        {MORSE_DASH, MORSE_DOT, MORSE_DELAY, MORSE_NULL, MORSE_NULL}, // n
        {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DELAY, MORSE_NULL}, // o
        {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DELAY}, // p
        {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DELAY}, // q
        {MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_DELAY, MORSE_NULL}, // r
        {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DELAY, MORSE_NULL}, // s
        {MORSE_DASH, MORSE_DELAY, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // t
        {MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DELAY, MORSE_NULL}, // u
        {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DELAY}, // v
        {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DELAY, MORSE_NULL}, // w
        {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DELAY}, // x
        {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DELAY}, // y
        {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DELAY}  // z
};

static morse_signal_t signal_backing_buffer[64];
static struct cbuff signal_buffer;

// TODO : this can probably be 25 (at most), worst-case per-letter is 5, and we are loading
// TODO : 5 letter words, but adding a bit more for safety and to make it a power of 2
static morse_input_t action_backing_buffer[32];
static struct cbuff action_buffer;

void morse_init(morse_t morse) {
    cbuff_init(&signal_buffer, &signal_backing_buffer, 64, sizeof(morse_signal_t));
    cbuff_init(&action_buffer, &action_backing_buffer, 32, sizeof(morse_input_t));

    morse->signal_buffer = &signal_buffer;
    morse->morse_input_buffer = &action_buffer;
}

bool morse_append_signal(morse_t morse, signal_t signal, uint64_t timestamp) {
    if(signal == SIGNAL_VOID) {
        return false;
    }

    morse_signal_t head;
    size_t head_peeked = cbuff_peekhead(morse->signal_buffer, &head, 1);
    if(head_peeked == 1) {
        // there's no point attempting to check this if the buffer is empty
       if (signal == head.value) {
            return false;
        }
    }

    if(!cbuff_canwrite(morse->signal_buffer)) {
        return false;
    }

    morse_signal_t entry = (morse_signal_t) {
        .value = signal,
        .timestamp = timestamp
    };

    cbuff_write(morse->signal_buffer, &entry);
    return true;
}

bool morse_convert(morse_t morse, uint64_t timestamp) {
    bool processing_performed = false;
    size_t signal_buffer_size = cbuff_size(morse->signal_buffer);

    size_t processed_idx = 0;

    if(signal_buffer_size > 1) {
        morse_signal_t signals[signal_buffer_size];
        size_t readcount = cbuff_peektail(morse->signal_buffer, signals, signal_buffer_size);

        for(size_t i = 1; i < readcount; i++) {
            morse_signal_t prev = signals[i - 1];
            morse_signal_t current = signals[i];

            // find the difference in milliseconds from the last transition to the current one
            // and then convert that to "dits" which are the basic unit of how morse code
            // is determined...
            uint64_t duration  = current.timestamp - prev.timestamp;
            double dits = (double)duration / (double)MORSE_DIT_MS;

            morse_input_t input = MORSE_NULL;

            // we want to detect a transition from high to low, the timestamp records the
            // time that the transition occurred, so we can subtract
            if(prev.value == SIGNAL_HIGH && current.value == SIGNAL_LOW) {
                // TODO : this should probably be changed to be an "adaptive" value rather than
                // TODO : a fixed value, but *shrug*

                if(dits > MORSE_DOT_START && dits <= MORSE_DOT_END) {
                    input = MORSE_DOT;
                }
                else if(dits > MORSE_DOT_END && dits <= MORSE_DASH_END) {
                    input = MORSE_DASH;
                }
                else if(dits > MORSE_SHORT_HOLD_START && dits <= MORSE_SHORT_HOLD_END) {
                    input = MORSE_SHORT_HOLD;
                }
                else if(dits > MORSE_LONG_HOLD_START && dits <= MORSE_LONG_HOLD_END) {
                    input = MORSE_LONG_HOLD;
                }

                cbuff_write(morse->morse_input_buffer, &input);
                processed_idx = i;
            }
            // if we are going from LOW to HIGH and there's been a long enough break, prepend the DELAY into the buffer
            else if(prev.value == SIGNAL_LOW && current.value == SIGNAL_HIGH) {
                if(dits > MORSE_DELAY_START) {
                    input = MORSE_DELAY;

                    if(cbuff_size(morse->morse_input_buffer) > 0) {
                        morse_input_t head;
                        cbuff_peekhead(morse->morse_input_buffer, &head, 1);

                        // don't write a delay to the input buffer if there is already one there...
                        if(head != MORSE_DELAY) {
                            cbuff_write(morse->morse_input_buffer, &input);
                            processed_idx = i;
                        }
                    }
                    else {
                        cbuff_write(morse->morse_input_buffer, &input);
                        processed_idx = i;
                    }
                }
            }
        }

        if(processed_idx > 0) {
            cbuff_seek(morse->signal_buffer, processed_idx);
            processing_performed = true;
        }
    }

    if(cbuff_size(morse->signal_buffer) > 0) {
        // this is a quirk of how we are implementing the signal reading, as we only record the timestamp of the FIRST
        // time the signal changes in the buffer we then struggle to find when a delay has occurred between letters.
        // In morse code, the gap between a letter is considered 7 * MORSE_DIT_MS, so we need to look at the last value
        // that was written to the signal buffer, if it is a LOW signal, but it happened more than 7*MORSE_DIT_MS ago then
        // we need to artificially inject a MORSE_DELAY into the decoded buffer.
        morse_signal_t tail;
        cbuff_peektail(morse->signal_buffer, &tail, 1);

        // before we do the next check we need to see if there has already been a delay inserted into
        // the morse sequence, if so then we can just return, if not we need to process the next section to
        // check and insert the delay if it's required.
        if (cbuff_size(morse->morse_input_buffer) > 0) {
            morse_input_t head;
            cbuff_peekhead(morse->morse_input_buffer, &head, 1);

            if (head == MORSE_DELAY) {
                return processing_performed;
            }
        }

        // if we see that there is a LOW in the signal buffer, we need to potentially insert a DELAY into the morse
        // buffer based on how long it's been since we've seen the LOW value
        if (tail.value == SIGNAL_LOW) {
            uint64_t difference = timestamp - tail.timestamp;

            if (difference / MORSE_DIT_MS >= 7) {
                morse_input_t input = MORSE_DELAY;
                cbuff_write(morse->morse_input_buffer, &input);
                cbuff_seek(morse->signal_buffer, 1);
            }
        }
    }

    return processing_performed;
}


#define MAX_INPUTS_PER_LETTER 5
#define MAX_LETTERS 26
#define MINIMUM_INPUTS_TO_PROCESS 2

static inline uint8_t min(uint8_t a, uint8_t b) {
    return a < b ? a : b;
}

bool morse_decode(morse_t morse, morse_action_event_t* letter) {
    // default this to a NO-OP unless it's set to something else
    letter->type = MORSE_ACTION_NOOP;

    size_t buffer_length = cbuff_size(morse->morse_input_buffer);

    if(buffer_length == 0) {
        return false;
    }

    uint8_t offset = 0;
    morse_input_t temp;

    // we need to trim off the front of the list until there is something actionable at the front of the list
    cbuff_peektail_after(morse->morse_input_buffer, &temp, offset, 1);
    while((temp == MORSE_DELAY || temp == MORSE_NULL) && offset < buffer_length) {
        offset++;
        cbuff_peektail_after(morse->morse_input_buffer, &temp, offset, 1);
    }

    // now we have something actionable at the front of the queue, so we can start processing the entries
    morse_input_t inputs[MAX_INPUTS_PER_LETTER];

    for(uint8_t i = 0; i < min((buffer_length - offset), MAX_INPUTS_PER_LETTER); i++) {
        memset(&inputs, MORSE_NULL, MAX_INPUTS_PER_LETTER * sizeof(morse_input_t));
        cbuff_peektail_after(morse->morse_input_buffer, &inputs, offset, i + 1);

        if(inputs[i] == MORSE_SHORT_HOLD) {
            letter->type = MORSE_ACTION_BACKSPACE;
            cbuff_seek(morse->morse_input_buffer, i + 1);

            return true;
        }
        else if(inputs[i] == MORSE_LONG_HOLD) {
            letter->type = MORSE_ACTION_RESET;
            cbuff_seek(morse->morse_input_buffer, i + 1);

            return true;
        }
        else if(i > 0 && (inputs[i] == MORSE_DELAY)) {
            uint8_t start_idx = 0;

            // so we have a delay which means we need to process everything up to the delay
            // and compare it against the morse table to find the letter
            for(uint8_t j = 0; j < MAX_LETTERS; j++) {
                // check if the values match, if they do then we can assign the letter, otherwise we move
                // on to the next entry
                if (memcmp(&(inputs[start_idx]), morse_table[j], MAX_INPUTS_PER_LETTER * sizeof(morse_input_t)) == 0) {
                    letter->type = MORSE_ACTION_ADD_LETTER;
                    letter->ch = (char) (((uint8_t) 'A') + j);

                    // we have found a match, so we want to move the read pointer forward through
                    // the buffer so they aren't ingested in the future.
                    cbuff_seek(morse->morse_input_buffer, i + 1);
                    return true;
                }
            }

            // if we get to this point, we've found a delay which means we want to try and match against
            // the letters, but it doesnt match anything in the morse table, but we need to reject the
            // entries and move through the buffer, otherwise we will just spin at this point forever.
            cbuff_seek(morse->morse_input_buffer, i + 1);
            return false;
        }

    }

    // if we have hit the maximum size and we haven't found a delay, then throw away the head of the
    // buffer as there is no way we will ever move it otherwise and there's maybe a delay after that
    if(buffer_length >= MAX_INPUTS_PER_LETTER) {
        cbuff_seek(morse->morse_input_buffer, 1);
    }

    return false;
}

