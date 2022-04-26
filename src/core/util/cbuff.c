//
// Created by aidan on 26/04/2022.
//

#include "cbuff.h"

void cbuff_init(cbuff_t buff, void** buffer, size_t capacity, size_t itemsize) {
    buff->buffer = buffer;
    buff->capacity = capacity;
    buff->item_sz = itemsize;

    cbuff_clear(buff);
}

bool cbuff_write(cbuff_t buff, void* data) {
    if(!cbuff_canwrite(buff)) {
        return false;
    }

    memcpy(&(buff->buffer[buff->write_idx]), data, buff->item_sz);
    buff->write_idx = (buff->write_idx + 1 ) % buff->capacity;

    return true;
}

bool cbuff_read(cbuff_t buff, void* data) {
    if(!cbuff_canread(buff)) {
        return false;
    }

    memcpy(&data, &buff->buffer[buff->read_idx], buff->item_sz);
    buff->read_idx = (buff->read_idx + 1 ) % buff->capacity;

    return true;
}

void cbuff_clear(cbuff_t buff) {
    memset(buff->buffer, 0, buff->capacity * buff->item_sz);

    buff->read_idx = 0;
    buff->write_idx = 0;
}

bool cbuff_canread(cbuff_t buff) {
    if((buff->read_idx % buff->capacity) == buff->write_idx) {
        return false;
    }

    return true;
}

bool cbuff_canwrite(cbuff_t buff) {
    if((buff->write_idx + 1) % buff->capacity == buff->read_idx) {
        return false;
    }

    return true;
}
