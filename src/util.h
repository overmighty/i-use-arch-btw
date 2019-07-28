#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

char *buffer_from_file(FILE *file);

void *try_malloc(size_t size);

void *try_realloc(void *pointer, size_t size);

#endif
