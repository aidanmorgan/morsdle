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
void cbuff_clear(cbuff_t buff);

bool cbuff_canread(cbuff_t buff);
bool cbuff_canwrite(cbuff_t buff);

#endif //CBUFF_H
