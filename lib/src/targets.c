// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/targets.h"

#include "iuab/buffer.h"
#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/targets/bytecode.h"
#include "iuab/token.h"

#include <stdio.h>

const char *iuab_target_name(enum iuab_target target) {
    switch (target) { // NOLINT(hicpp-multiway-paths-covered)
    case IUAB_TARGET_BYTECODE: return "bytecode";
    default: return "???";
    }
}

enum iuab_error iuab_compile(
    enum iuab_target target,
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
) {
    switch (target) { // NOLINT(hicpp-multiway-paths-covered)
    case IUAB_TARGET_BYTECODE:
        return iuab_compile_bytecode(src, dst, last_token_dst);
    default: return IUAB_ERROR_INVALID_TARGET;
    }
}

enum iuab_error iuab_run(enum iuab_target target, struct iuab_context *ctx) {
    switch (target) { // NOLINT(hicpp-multiway-paths-covered)
    case IUAB_TARGET_BYTECODE: return iuab_run_bytecode(ctx);
    default: return IUAB_ERROR_INVALID_TARGET;
    }
}
