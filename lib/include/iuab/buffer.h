#ifndef IUAB_BUFFER_H
#define IUAB_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "errors.h"

#include <stddef.h>
#include <stdint.h>

/* A byte buffer that automatically grows in capacity. */
struct iuab_buffer {
    size_t len; /* The length of the stored data, in bytes. */
    size_t cap; /* The allocated storage capacity, in bytes. */
    uint8_t *data; /* The array used to store the data. */
};

/* Initializes the buffer pointed to by the given pointer. */
enum iuab_error iuab_buffer_init(struct iuab_buffer *buffer);

/* Trims the given buffer's capacity down to its current length. */
void iuab_buffer_trim(struct iuab_buffer *buffer);

/* Writes the given byte at the end of the given buffer. */
void iuab_buffer_put_byte(struct iuab_buffer *buffer, uint8_t byte);

/* Writes the given size at the end of the given buffer. */
void iuab_buffer_put_size(struct iuab_buffer *buffer, size_t size);

/* Cleans up the given buffer. */
void iuab_buffer_fini(struct iuab_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_BUFFER_H */
