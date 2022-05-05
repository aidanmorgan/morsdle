#include "morse.h"

static morse_signal_t signal_backing_buffer[25];
static struct cbuff signal_buffer;

static morse_input_t morse_backing_buffer[25];
static struct cbuff morse_buffer;

void morse_init(morse_t morse,  bool (*callback)(morse_t morse, char*c)) {
    cbuff_init(&signal_buffer, &signal_backing_buffer, 25, sizeof(morse_signal_t));
    cbuff_init(&morse_buffer, &morse_backing_buffer, 25, sizeof(morse_input_t));

    morse->signal_buffer = &signal_buffer;
    morse->morse_buffer = &morse_buffer;
    morse->parse_callback = callback;
}

bool morse_append_signal(morse_t morse, signal_t signal, uint64_t timestamp) {
    if(signal == SIGNAL_VOID) {
        return false;
    }

    morse_signal_t tail;
    cbuff_peekhead(morse->signal_buffer, &tail, 1);

    if(signal == tail.value) {
        return false;
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

bool morse_process_signals(morse_t morse, uint64_t timestamp) {
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
            morse_input_t input = MORSE_NULL;

            // we want to detect a transition from high to low, the timestamp records the
            // time that the transition occurred, so we can subtract
            if(prev.value == SIGNAL_HIGH && current.value == SIGNAL_LOW) {
                // TODO : this should probably be changed to be an "adaptive" value rather than
                // TODO : a fixed 500ms value, but *shrug*
                double dits = (double)duration / (double)MORSE_DIT_MS;


                if(dits > MORSE_DOT_START && dits <= MORSE_DOT_END) {
                    input = MORSE_DOT;
                }
                else if(dits > MORSE_DOT_END && dits <= MORSE_DASH_END) {
                    input = MORSE_DASH;
                }

                cbuff_write(morse->morse_buffer, &input);
                processed_idx = i;
            }
            else if(current.value == SIGNAL_LOW && duration > MORSE_DASH_END){
                    input = MORSE_DELAY;
                    cbuff_write(morse->morse_buffer, &input);
                    processed_idx = i;
            }
        }

        if(processed_idx > 0) {
            cbuff_seek(morse->signal_buffer, processed_idx);
            processing_performed = true;
        }
    }

    morse_signal_t peeked;
    cbuff_peektail(morse->signal_buffer, &peeked, 1);

    if(peeked.value == SIGNAL_LOW) {
        uint64_t difference = timestamp - peeked.timestamp;

        if(difference / MORSE_DIT_MS >= 7) {
            morse_input_t input = MORSE_DELAY;
            cbuff_write(morse->morse_buffer, &input);

            cbuff_seek(morse->signal_buffer, 1);
        }
    }

    return processing_performed;
}

const static morse_input_t morse_table[26][5] = {
    {MORSE_DOT, MORSE_DASH, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // a
    {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_NULL}, // b
    {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_NULL}, // c
    {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_NULL, MORSE_NULL}, // d
    {MORSE_DOT, MORSE_NULL, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // e
    {MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_NULL}, // f
    {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_NULL, MORSE_NULL}, // g
    {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_NULL}, // h
    {MORSE_DOT, MORSE_DOT, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // i
    {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_NULL}, // j
    {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_NULL, MORSE_NULL}, // k
    {MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_NULL}, // l
    {MORSE_DASH, MORSE_DASH, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // m
    {MORSE_DASH, MORSE_DOT, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // n
    {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_NULL, MORSE_NULL}, // o
    {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_NULL}, // p
    {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_NULL}, // q
    {MORSE_DOT, MORSE_DASH, MORSE_DOT, MORSE_NULL, MORSE_NULL}, // r
    {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_NULL, MORSE_NULL}, // s
    {MORSE_DASH, MORSE_NULL, MORSE_NULL, MORSE_NULL, MORSE_NULL}, // t
    {MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_NULL, MORSE_NULL}, // u
    {MORSE_DOT, MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_NULL}, // v
    {MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_NULL, MORSE_NULL}, // w
    {MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_DASH, MORSE_NULL}, // x
    {MORSE_DASH, MORSE_DOT, MORSE_DASH, MORSE_DASH, MORSE_NULL}, // y
    {MORSE_DASH, MORSE_DASH, MORSE_DOT, MORSE_DOT, MORSE_NULL}  // z
};

#define MAX_INPUTS_PER_LETTER 5
#define MAX_LETTERS 26

bool morse_process_input(morse_t morse, char* result) {
    if(cbuff_size(morse->morse_buffer) < 2) {
        return false;
    }

    size_t buffer_length = cbuff_size(morse->morse_buffer);
    if(buffer_length < 2) {
        return false;
    }

    morse_input_t inputs[MAX_INPUTS_PER_LETTER];

    for(uint8_t i = 2; i < MAX_INPUTS_PER_LETTER + 1; i++) {
        memset(&inputs, MORSE_NULL, MAX_INPUTS_PER_LETTER * sizeof(morse_input_t));
        cbuff_peektail(morse->morse_buffer, &inputs, i);

        if(inputs[i - 1] == MORSE_DELAY) {
            inputs[i - 1] = MORSE_NULL;

            // so we have a delay which means we need to process everything up to the delay
            // and compare it against the morse table to find the letter
            for(uint8_t j = 0; j < MAX_LETTERS; j++) {
                const morse_input_t* table_entry = morse_table[j];

                // check if the values match, if they do then we can assign the letter, otherwise we move
                // on to the next entry
                if(memcmp(&inputs, table_entry, MAX_INPUTS_PER_LETTER * sizeof(morse_input_t)) == 0) {
                    *result = (char)(((uint8_t)'A') + j);
                    return true;
                }

            }
        }
    }

    return false;
}

