#ifndef IUAB_BUFFER_H
#define IUAB_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "errors.h"

#include <stddef.h>
#include <stdint.h>

/* A dynamically sized 8-bit byte buffer. */
struct iuab_buffer {
    size_t len; /* The length of the stored data, in bytes. */
    size_t cap; /* The allocated storage capacity, in bytes. */
    uint8_t *data; /* A pointer to the first byte of stored data. */
};

/* Initializes the given buffer. */
enum iuab_error iuab_buffer_init(struct iuab_buffer *buffer);

/* Sets the given buffer's capacity to its length. */
void iuab_buffer_trim(struct iuab_buffer *buffer);

/* Writes the given byte at the end of the given buffer. */
void iuab_buffer_put_byte(struct iuab_buffer *buffer, uint8_t byte);

/* Writes the given size at the end of the given buffer. */
void iuab_buffer_put_size(struct iuab_buffer *buffer, size_t size);

/* Destroys the given buffer. */
void iuab_buffer_fini(struct iuab_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_BUFFER_H */
