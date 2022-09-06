// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/context.h"

#include <string.h>

void iuab_context_init(
    struct iuab_context *ctx,
    const uint8_t *program,
    FILE *in,
    FILE *out,
    void (*debug_handler)(struct iuab_context *)
) {
    ctx->ip = program;
    ctx->dp = ctx->memory;
    ctx->in = in;
    ctx->out = out;
    ctx->debug_handler = debug_handler;
    ctx->program = program;
    memset(ctx->memory, 0, IUAB_CONTEXT_MEMORY_SIZE);
}
