//
// Created by aidan on 26/04/2022.
//

#ifndef CBUFF_H
#define CBUFF_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct cbuff {
    void * buffer;

    // index into the buffer for where we have read up to
    size_t read_idx;
    // index into the buffer for where we have written up to
    size_t write_idx;
    // the current unread size of the contents of the buffer
    size_t size;

    // the maximum capacity of the buffer
    size_t capacity;

    // the size of each item in the buffer
    size_t item_sz;
};

typedef struct cbuff* cbuff_t;

void cbuff_init(cbuff_t buff, void* buffer, size_t capacity, size_t itemsize);
bool cbuff_write(cbuff_t buff, const void* data);
bool cbuff_read(cbuff_t buff, void* data);
size_t cbuff_readmany(cbuff_t buff, void* data, size_t count);
bool cbuff_clear(cbuff_t buff);

// copies the data at the current read pointer in the buffer into the provided pointer
size_t cbuff_peekhead(cbuff_t buff, void* data, size_t count);
// copies count items from the current write pointer -1 to the provided pointer
size_t cbuff_peektail(cbuff_t buff, void* data, size_t count);

// moves the read index in the circular buffer forward count items
bool cbuff_seek(cbuff_t buff, size_t count);

// returns the current size of the buffer
size_t cbuff_size(cbuff_t buff);

// returns TRUE if there is data available to be read from the buffer, FALSE otherwise
bool cbuff_canread(cbuff_t buff);

// returns TRUE if there is space in the buffer to write data, FALSE otherwise
bool cbuff_canwrite(cbuff_t buff);

#endif //CBUFF_H
