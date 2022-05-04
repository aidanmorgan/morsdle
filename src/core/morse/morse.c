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

bool morse_append(morse_t morse, morse_signal_t signal) {
    if(signal.value == SIGNAL_VOID) {
        return false;
    }

    morse_signal_t tail;
    cbuff_peekhead(morse->signal_buffer, &tail, 1);

    if(signal.value == tail.value) {
        return false;
    }

    if(!cbuff_canwrite(morse->signal_buffer)) {
        return false;
    }

    cbuff_write(morse->signal_buffer, &signal);
    return true;
}

bool morse_process_signals(morse_t morse) {
    size_t val = cbuff_size(morse->signal_buffer);

    size_t processed_idx = 0;

    if(val >= 2) {
        morse_signal_t signals[val];
        size_t readcount = cbuff_peektail(morse->signal_buffer, signals, val);

        for(size_t i = 1; i < readcount; i++) {
            morse_signal_t prev = signals[i - 1];
            morse_signal_t current = signals[i];

            // we want to detect a transition from high to low, the timestamp records the
            // time that the transition occurred, so we can subtract
            if(prev.value == SIGNAL_HIGH && current.value == SIGNAL_LOW) {
                // find the difference in milliseconds from the last transition to the current one
                // and then convert that to "dits" which are the basic unit of how morse code
                // is determined...
                uint64_t duration  = current.timestamp - prev.timestamp;

                // TODO : this should probably be changed to be an "adaptive" value rather than
                // TODO : a fixed 500ms value, but *shrug*
                double dits = (double)duration / (double)MORSE_DIT_MS;

                morse_input_t input = MORSE_NULL;

                if(dits > MORSE_DOT_START && dits <= MORSE_DOT_END) {
                    input = MORSE_DOT;
                }
                else if(dits > MORSE_DOT_END && dits <= MORSE_DASH_END) {
                    input = MORSE_DASH;
                }

                cbuff_write(morse->morse_buffer, &input);

                processed_idx = i;
            }
        }

        if(processed_idx > 0) {
            cbuff_seek(morse->signal_buffer, processed_idx);
            return true;
        }
    }

    return false;
}

void morse_process_input(morse_t morse) {

}

