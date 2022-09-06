// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_TARGETS_H
#define IUAB_TARGETS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer.h"
#include "context.h"
#include "errors.h"
#include "token.h"

#include <stdio.h>

// An I use Arch btw compilation target.
enum iuab_target {
    // I use Arch btw bytecode.
    IUAB_TARGET_BYTECODE,
};

// Returns the name of the given target as a string.
const char *iuab_target_name(enum iuab_target target);

// Compiles for the given target the source file pointed to by `src` into
// code to write to the buffer pointed to by `dst` and writes the last token
// processed at the location pointed to by `last_token_dst`. Returns the error
// that occurred in the process.
enum iuab_error iuab_compile(
    enum iuab_target target,
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
);

// Runs the program compiled for the given target from the context pointed to
// by `ctx`.
enum iuab_error iuab_run(enum iuab_target target, struct iuab_context *ctx);

#ifdef __cplusplus
};
#endif

#endif // IUAB_TARGETS_H
