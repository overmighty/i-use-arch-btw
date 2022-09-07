// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_CONTEXT_H
#define IUAB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

// The size of the working memory stored in an I use Arch btw program context.
#define IUAB_CONTEXT_MEMORY_SIZE (1U << 16U)

// An I use Arch btw program context.
struct iuab_context {
    const uint8_t *ip;
    uint8_t *dp;
    FILE *in;
    FILE *out;
    void (*debug_handler)(struct iuab_context *);
    const uint8_t *program;
    uint8_t memory[IUAB_CONTEXT_MEMORY_SIZE];
};

// Initializes the given context for execution of the code stored at `program`,
// with the files pointed to by `in` and `out as input and output files
// respectively, and the function pointed to by `debug_handler` as debugging
// event handler.
void iuab_context_init(
    struct iuab_context *ctx,
    const uint8_t *program,
    FILE *in,
    FILE *out,
    void (*debug_handler)(struct iuab_context *)
);

#ifdef __cplusplus
}
#endif

#endif // IUAB_CONTEXT_H
