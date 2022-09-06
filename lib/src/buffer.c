// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/buffer.h"

#include "iuab/errors.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define IUAB_BUFFER_DEFAULT_CAP 64
#define IUAB_BUFFER_GROWTH_FACTOR 2

enum iuab_error iuab_buffer_init(struct iuab_buffer *buffer) {
    buffer->size = 0;
    buffer->cap = IUAB_BUFFER_DEFAULT_CAP;
    buffer->data = malloc(buffer->cap);

    if (!buffer->data) {
        return IUAB_ERROR_MALLOC;
    }

    return IUAB_ERROR_SUCCESS;
}

enum iuab_error
iuab_buffer_write(struct iuab_buffer *buffer, const void *data, size_t n) {
    if (buffer->size + n > buffer->cap) {
        buffer->cap *=
            (buffer->cap + n - 1) / buffer->cap * IUAB_BUFFER_GROWTH_FACTOR;
        uint8_t *new_data = realloc(buffer->data, buffer->cap);

        if (!new_data) {
            return IUAB_ERROR_MALLOC;
        }

        buffer->data = new_data;
    }

    memcpy(&buffer->data[buffer->size], data, n);
    buffer->size += n;
    return IUAB_ERROR_SUCCESS;
}

size_t iuab_buffer_pop_size(struct iuab_buffer *buffer) {
    size_t top = ((size_t *) &buffer->data[buffer->size])[-1];
    buffer->size -= sizeof(top);
    return top;
}

void iuab_buffer_fini(struct iuab_buffer *buffer) {
    free(buffer->data);
}
