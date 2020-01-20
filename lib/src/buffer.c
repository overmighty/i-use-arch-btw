#include <iuab/buffer.h>

#include <stdlib.h> /* for malloc, realloc */

/* Initial/base buffer capacity. */
#define BASE_CAP 64
/* Capacity growth factor for buffers. */
#define GROWTH_FACTOR 2

/* Reallocates the data member of the iuab_buffer struct passed by pointer.
 * Used to "save" a capacity change. */
#define REALLOC_DATA(buf) ((buf)->data = realloc((buf)->data, (buf)->cap))

enum iuab_error iuab_buffer_init(struct iuab_buffer *buffer) {
    buffer->len = 0;
    buffer->cap = BASE_CAP;
    buffer->data = malloc(buffer->cap);

    if (!buffer->data) {
        return IUAB_ERROR_MEMORY;
    }

    return IUAB_ERROR_SUCCESS;
}

void iuab_buffer_trim(struct iuab_buffer *buffer) {
    buffer->cap = buffer->len;
    REALLOC_DATA(buffer);
}

static void iuab_buffer_grow(struct iuab_buffer *buffer) {
    buffer->cap *= GROWTH_FACTOR;
    REALLOC_DATA(buffer);
}

void iuab_buffer_put_byte(struct iuab_buffer *buffer, uint8_t byte) {
    if (buffer->len >= buffer->cap) {
        iuab_buffer_grow(buffer);
    }

    if (buffer->data) {
        buffer->data[buffer->len++] = byte;
    }
}

void iuab_buffer_put_size(struct iuab_buffer *buffer, size_t size) {
    while (buffer->len + sizeof(size_t) > buffer->cap) {
        iuab_buffer_grow(buffer);
    }

    if (buffer->data) {
        *((size_t *) &buffer->data[buffer->len]) = size;
        buffer->len += sizeof(size);
    }
}

void iuab_buffer_fini(struct iuab_buffer *buffer) {
    free(buffer->data);
}
