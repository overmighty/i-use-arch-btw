#include <stdlib.h>
#include "util.h"

static long get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size;
}

char *buffer_from_file(FILE *file) {
    long file_size = get_file_size(file);
    char *buffer = try_malloc((file_size + 1) * sizeof(char));
    fread(buffer, sizeof(char), file_size, file);
    buffer[file_size] = '\0';
    return buffer;
}

static void validate_pointer(const void *pointer, const char *message) {
    if (pointer == NULL) {
        fputs(message, stderr);
        exit(EXIT_FAILURE);
    }
}

void *try_malloc(size_t size) {
    void *pointer = malloc(size);
    validate_pointer(pointer, "error: memory allocation failed\n");
    return pointer;
}

void *try_realloc(void *pointer, size_t size) {
    void *new_pointer = realloc(pointer, size);
    validate_pointer(new_pointer, "error: memory reallocation failed\n");
    return new_pointer;
}
