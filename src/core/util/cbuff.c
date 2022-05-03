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


bool cbuff_clear(cbuff_t buff) {
    buff->read_idx = 0;
    buff->write_idx = 0;
    buff->size = 0;

    memset(buff->buffer, 0, buff->item_sz * buff->capacity);
    return true;
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

size_t cbuff_size(cbuff_t buff) {
    return buff->size;
}

size_t cbuff_readmany(cbuff_t buff, void* result, size_t count) {
    if(!cbuff_canread(buff)) {
        return 0;
    }

    // if we attempt to read more entries than the capacity of the buffer, then clamp the
    // request to the maximum size of the buffer
    if(count > buff->capacity) {
        count = buff->capacity;
    }

    // check to see how many items we are attempting to read, if it's less than the amount
    // of entries available then clamp the read attempt to that value.
    //
    // we'll return the number we actually read anyway
    if(count > buff->size) {
        count = buff->size;
    }

    // grab a pointer to the first element of the array
    void* array = result;

    for(size_t i = 0; i < count; i++) {
        if(!cbuff_read(buff, array)) {
            return i;
        }

        array = array + (buff->item_sz);
    }

    return count;
}

size_t cbuff_peektail(cbuff_t buff, void* result, size_t count) {
    if(count > buff->capacity) {
        count = buff->capacity;
    }

    if(count > buff->size) {
        count = buff->size;
    }

    void* array = result;
    size_t readindex = buff->read_idx;

    for(size_t i = 0; i < count; i++) {
        void* entry = buff->buffer + (readindex * buff->item_sz);

        memcpy(array, entry, buff->item_sz);
        readindex++;

        if(readindex >= buff->capacity) {
            readindex = 0;
        }

        array = array + (buff->item_sz);
    }

    return count;
}

size_t cbuff_peekhead(cbuff_t buff, void* result, size_t count) {
    if(count > buff->capacity) {
        count = buff->capacity;
    }

    if(count > buff->size) {
        count = buff->size;
    }

    void* array = result;
    // can't use a size_t here as we're potentially going into negative numbers
    int64_t idx = buff->write_idx - count;

    // prevent an underrun if the number is negative, so move back to the tail-end
    if(idx < 0) {
        idx = buff->capacity + idx;
    }

    for(size_t i = 0; i < count; i++) {
        void* entry = buff->buffer + (idx * buff->item_sz);

        memcpy(array, entry, buff->item_sz);
        idx++;

        if(idx >= buff->capacity) {
            idx = 0;
        }

        array = array + (buff->item_sz);
    }

    return count;
}

bool cbuff_seek(cbuff_t buff, size_t count) {
    if(count > buff->size) {
        return false;
    }

    if(count > buff->capacity) {
        return false;
    }

    buff->read_idx = (buff->read_idx + count) % buff->capacity;
    return true;
}

