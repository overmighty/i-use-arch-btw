// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_BUFFER_H
#define IUAB_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "errors.h"

#include <stddef.h>
#include <stdint.h>

// A growable byte buffer.
struct iuab_buffer {
    size_t size;
    size_t cap;
    uint8_t *data;
};

// Initializes the given buffer. Returns the error that occurred in the process.
enum iuab_error iuab_buffer_init(struct iuab_buffer *buffer);

// Writes `n` bytes from `data` at the end of the given buffer. Returns the
// error that occurred in the process.
enum iuab_error
iuab_buffer_write(struct iuab_buffer *buffer, const void *data, size_t n);

// Writes the given `uint8_t` value at the end of the given buffer. Returns the
// error that occurred in the process.
static inline enum iuab_error
iuab_buffer_write_u8(struct iuab_buffer *buffer, uint8_t value) {
    return iuab_buffer_write(buffer, &value, sizeof(value));
}

// Writes the given `size_t` value at the end of the given buffer. Returns the
// error that occurred in the process.
static inline enum iuab_error
iuab_buffer_write_size(struct iuab_buffer *buffer, size_t value) {
    return iuab_buffer_write(buffer, &value, sizeof(value));
}

// Transforms to a call to `iuab_buffer_write()` with `sizeof(data)` as argument
// for the `n` parameter.
#define IUAB_BUFFER_WRITE(buffer, data) \
    iuab_buffer_write((buffer), (data), sizeof(data))

// Returns and removes the `size_t` value at the end of the given buffer.
size_t iuab_buffer_pop_size(struct iuab_buffer *buffer);

// Finalizes the given buffer. Frees the allocated storage cap.
void iuab_buffer_fini(struct iuab_buffer *buffer);

#ifdef __cplusplus
};
#endif

#endif // IUAB_BUFFER_H
