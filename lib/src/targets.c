// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/targets.h"

#include "iuab/buffer.h"
#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/targets/bytecode.h"
#include "iuab/targets/jit_x86_64.h"
#include "iuab/token.h"

#include <stdio.h>

const char *iuab_target_name(enum iuab_target target) {
    switch (target) {
    case IUAB_TARGET_BYTECODE: return "bytecode";
    case IUAB_TARGET_JIT_X86_64: return "JIT x86-64";
    default: return "???";
    }
}

bool iuab_target_is_jit(enum iuab_target target) {
    return target == IUAB_TARGET_JIT_X86_64;
}

enum iuab_error iuab_compile(
    enum iuab_target target,
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
) {
    switch (target) {
    case IUAB_TARGET_BYTECODE:
        return iuab_compile_bytecode(src, dst, last_token_dst);
    case IUAB_TARGET_JIT_X86_64:
        return iuab_compile_jit_x86_64(src, dst, last_token_dst);
    default: return IUAB_ERROR_INVALID_TARGET;
    }
}

enum iuab_error iuab_run(enum iuab_target target, struct iuab_context *ctx) {
    switch (target) {
    case IUAB_TARGET_BYTECODE: return iuab_run_bytecode(ctx);
    case IUAB_TARGET_JIT_X86_64: return iuab_run_jit_x86_64(ctx);
    default: return IUAB_ERROR_INVALID_TARGET;
    }
}
