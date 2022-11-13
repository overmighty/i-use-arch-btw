// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/targets/bytecode.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static enum iuab_error iuab_bytecode_run_addp(struct iuab_context *ctx) {
    uint16_t operand;
    memcpy(&operand, ctx->ip, sizeof(operand));

    if (ctx->dp - ctx->memory >= IUAB_CONTEXT_MEMORY_SIZE - operand) {
        return IUAB_ERROR_DP_OUT_OF_BOUNDS;
    }

    ctx->ip += sizeof(operand);
    ctx->dp += operand;
    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error iuab_bytecode_run_subp(struct iuab_context *ctx) {
    uint16_t operand;
    memcpy(&operand, ctx->ip, sizeof(operand));

    if (ctx->dp - ctx->memory < operand) {
        return IUAB_ERROR_DP_OUT_OF_BOUNDS;
    }

    ctx->ip += sizeof(operand);
    ctx->dp -= operand;
    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error iuab_bytecode_run_write(struct iuab_context *ctx) {
    int result = fputc(*ctx->dp, ctx->out);

    if (result == EOF) {
        return IUAB_ERROR_IO;
    }

    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error iuab_bytecode_run_read(struct iuab_context *ctx) {
    int result = fgetc(ctx->in);

    if (result == EOF) {
        if (ferror(ctx->in)) {
            return IUAB_ERROR_IO;
        }

        return IUAB_ERROR_RUNTIME_END_OF_INPUT_FILE;
    }

    *ctx->dp = result;
    return IUAB_ERROR_SUCCESS;
}

static void iuab_bytecode_run_jmpz(struct iuab_context *ctx) {
    size_t offset;

    if (*ctx->dp != 0) {
        ctx->ip += sizeof(offset);
        return;
    }

    memcpy(&offset, ctx->ip, sizeof(offset));
    ctx->ip = ctx->program + offset;
}

static void iuab_bytecode_run_jmpnz(struct iuab_context *ctx) {
    size_t offset;

    if (*ctx->dp == 0) {
        ctx->ip += sizeof(offset);
        return;
    }

    memcpy(&offset, ctx->ip, sizeof(offset));
    ctx->ip = ctx->program + offset;
}

enum iuab_error iuab_run_bytecode(struct iuab_context *ctx) {
    uint8_t op;

    while ((op = *ctx->ip++) != IUAB_BYTECODE_OP_RET) {
        enum iuab_error err = IUAB_ERROR_SUCCESS;

        switch (op) {
        case IUAB_BYTECODE_OP_ADDP: err = iuab_bytecode_run_addp(ctx); break;
        case IUAB_BYTECODE_OP_SUBP: err = iuab_bytecode_run_subp(ctx); break;
        case IUAB_BYTECODE_OP_ADDV: *ctx->dp += *ctx->ip++; break;
        case IUAB_BYTECODE_OP_SUBV: *ctx->dp -= *ctx->ip++; break;
        case IUAB_BYTECODE_OP_WRITE: err = iuab_bytecode_run_write(ctx); break;
        case IUAB_BYTECODE_OP_READ: err = iuab_bytecode_run_read(ctx); break;
        case IUAB_BYTECODE_OP_JMPZ: iuab_bytecode_run_jmpz(ctx); break;
        case IUAB_BYTECODE_OP_JMPNZ: iuab_bytecode_run_jmpnz(ctx); break;
        case IUAB_BYTECODE_OP_DEBUG: ctx->debug_handler(ctx); break;
        default: return IUAB_ERROR_BYTECODE_INVALID_OP;
        }

        if (err != IUAB_ERROR_SUCCESS) {
            return err;
        }
    }

    return IUAB_ERROR_SUCCESS;
}
