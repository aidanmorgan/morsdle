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

    void* entry = buff->buffer + (buff->write_idx * buff->item_sz);
    memcpy(entry, data, buff->item_sz);

    buff->write_idx++;

    if(buff->write_idx >= buff->capacity) {
        buff->write_idx = 0;
    }

    buff->size++;

    return true;
}

bool cbuff_read(cbuff_t buff, void* data) {
    if(!cbuff_canread(buff)) {
        return false;
    }

    void* entry = buff->buffer + (buff->read_idx * buff->item_sz);

    memcpy(data, entry, buff->item_sz);
    buff->read_idx++;

    if(buff->read_idx >= buff->capacity) {
        buff->read_idx = 0;
    }

    buff->size--;

    return true;
}

void cbuff_clear(cbuff_t buff) {
    buff->read_idx = 0;
    buff->write_idx = 0;

    memset(buff->buffer, 0, buff->item_sz * buff->capacity);
}

bool cbuff_canread(cbuff_t buff) {
    if(buff->size == 0) {
        return false;
    }

    return buff->size > 0;
}

bool cbuff_canwrite(cbuff_t buff) {
    if(buff->size == 0) {
        return true;
    }

    return buff->size < buff->capacity;
}
