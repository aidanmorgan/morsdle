//
// Created by aidan on 26/04/2022.
//

#include "cbuff.h"

void cbuff_init(cbuff_t buff, void* buffer, size_t capacity, size_t itemsize) {
    buff->buffer = buffer;
    buff->capacity = capacity;
    buff->item_sz = itemsize;

    cbuff_clear(buff);
}

#define BUFFER_START(x) (x->buffer)
#define BUFFER_END(x) (x->buffer + (x->item_sz * x->capacity))

bool cbuff_write(cbuff_t buff, const void* data) {
    if(!cbuff_canwrite(buff)) {
        return false;
    }

    memcpy(buff->head, data, buff->item_sz);

    buff->head = buff->head + buff->item_sz;
    if(buff->head == BUFFER_END(buff)) {
        buff->head = BUFFER_START(buff);
    }

    buff->count++;

    return true;
}

bool cbuff_read(cbuff_t buff, void* data) {
    if(!cbuff_canread(buff)) {
        return false;
    }

    memcpy(data, buff->tail, buff->item_sz);
    buff->tail = buff->tail + buff->item_sz;
    if(buff->tail == BUFFER_END(buff)) {
        buff->tail = buff->buffer;
    }

    buff->count--;

    return true;
}

void cbuff_clear(cbuff_t buff) {
    buff->count = 0;
    buff->head = buff->buffer;
    buff->tail = buff->buffer;

    memset(buff->buffer, 0, buff->item_sz * buff->capacity);
}

bool cbuff_canread(cbuff_t buff) {
    return buff->count > 0;
}

bool cbuff_canwrite(cbuff_t buff) {
    return buff->count < buff->capacity;
}
