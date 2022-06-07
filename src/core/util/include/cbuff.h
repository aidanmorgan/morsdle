//
// Created by aidan on 26/04/2022.
//

#ifndef CBUFF_H
#define CBUFF_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
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
} cbuff_t;

// initialises a circular buffer with the provided backing memory storage
void cbuff_init(cbuff_t* buff, void* buffer, size_t capacity, size_t itemsize);

// add the provided element to the buffer at the next head pointer
// WILL COPY the data into the buffer rather than share the pointer
bool cbuff_write(cbuff_t* buff, const void* data);

// read the next element from the tail pointer of the list
// WILL COPY the data into the provided pointer
bool cbuff_read(cbuff_t* buff, void* data);

// reads the next count elements from the tail pointer of the list, returning the number of items that
// were actually read.
// If the count is higher than the count of the data in the buffer, will read count elements
// If the count is higher than the maximum size of the buffer, will read the maximum size of elements
size_t cbuff_readmany(cbuff_t* buff, void* data, size_t count);

// will clear the data in the buffer, resetting the head and tail pointers
// WILL CLEAR the memory backing the buffer
bool cbuff_clear(cbuff_t* buff);

// copies the data at the current write pointer in the buffer into the provided pointer
size_t cbuff_peekhead(cbuff_t* buff, void* data, size_t count);
// copies count items from the current read pointer to the provided pointer
size_t cbuff_peektail(cbuff_t* buff, void* data, size_t count);
// skips idx items into the buffer from the current read pointer, then reads off num items
size_t cbuff_peektail_after(cbuff_t* buff, void* result, size_t idx, size_t num);

// moves the read index in the circular buffer forward count items
bool cbuff_seek(cbuff_t* buff, size_t count);

// returns the current size of the buffer
size_t cbuff_size(cbuff_t* buff);

// returns TRUE if there is data available to be read from the buffer, FALSE otherwise
bool cbuff_canread(cbuff_t* buff);

// returns TRUE if there is space in the buffer to write data, FALSE otherwise
bool cbuff_canwrite(cbuff_t* buff);

#endif //CBUFF_H
