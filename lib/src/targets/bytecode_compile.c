// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/buffer.h"
#include "iuab/errors.h"
#include "iuab/lexer.h"
#include "iuab/targets/bytecode.h"
#include "iuab/token.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DEFINE_IUAB_BYTECODE_INSTR_TYPE_WITH_OPERAND(name, operand_type) \
    typedef uint8_t name[1 + sizeof(operand_type)];                      \
                                                                         \
    void name##_init(name instr, uint8_t op, operand_type operand) {     \
        instr[0] = op;                                                   \
        memcpy(&instr[1], &operand, sizeof(operand));                    \
    }

DEFINE_IUAB_BYTECODE_INSTR_TYPE_WITH_OPERAND(iuab_bytecode_instr_u8, uint8_t)
DEFINE_IUAB_BYTECODE_INSTR_TYPE_WITH_OPERAND(iuab_bytecode_instr_u16, uint16_t)
DEFINE_IUAB_BYTECODE_INSTR_TYPE_WITH_OPERAND(iuab_bytecode_instr_size, size_t)

struct iuab_bytecode_compiler {
    struct iuab_lexer lexer;
    struct iuab_token token;
    struct iuab_buffer loop_stack;
    struct iuab_buffer *dst;
};

static enum iuab_error iuab_bytecode_compiler_init(
    struct iuab_bytecode_compiler *compiler,
    FILE *src,
    struct iuab_buffer *dst
) {
    compiler->dst = dst;
    iuab_lexer_init(&compiler->lexer, src);
    compiler->token = iuab_lexer_next_token(&compiler->lexer);
    return iuab_buffer_init(&compiler->loop_stack);
}

static void iuab_bytecode_compiler_fini(struct iuab_bytecode_compiler *compiler
) {
    iuab_buffer_fini(&compiler->loop_stack);
}

static enum iuab_error
iuab_bytecode_emit_additive_p(struct iuab_bytecode_compiler *compiler) {
    enum iuab_token_type token_type = compiler->token.type;
    struct iuab_lexer *lexer = &compiler->lexer;

    uint8_t op;

    switch (token_type) {
    case IUAB_TOKEN_I: op = IUAB_BYTECODE_OP_ADDP; break;
    case IUAB_TOKEN_USE: op = IUAB_BYTECODE_OP_SUBP; break;
    default: return IUAB_ERROR_COMPILER_INTERNAL;
    }

    uint16_t operand = 1;
    struct iuab_token next_token;

    while ((next_token = iuab_lexer_next_token(lexer)).type == token_type) {
        if (operand == UINT16_MAX) {
            compiler->token = next_token;
            return IUAB_ERROR_DP_OUT_OF_BOUNDS;
        }

        operand++;
    }

    compiler->token = next_token;

    iuab_bytecode_instr_u16 instr;
    iuab_bytecode_instr_u16_init(instr, op, operand);
    return IUAB_BUFFER_WRITE(compiler->dst, instr);
}

static enum iuab_error
iuab_bytecode_emit_additive_v(struct iuab_bytecode_compiler *compiler) {
    enum iuab_token_type token_type = compiler->token.type;
    struct iuab_lexer *lexer = &compiler->lexer;

    uint8_t op;

    switch (token_type) {
    case IUAB_TOKEN_ARCH: op = IUAB_BYTECODE_OP_ADDV; break;
    case IUAB_TOKEN_LINUX: op = IUAB_BYTECODE_OP_SUBV; break;
    default: return IUAB_ERROR_COMPILER_INTERNAL;
    }

    uint8_t operand = 1;
    struct iuab_token next_token;

    while ((next_token = iuab_lexer_next_token(lexer)).type == token_type) {
        operand++;
    }

    compiler->token = next_token;

    if (operand == 0) {
        return IUAB_ERROR_SUCCESS;
    }

    iuab_bytecode_instr_u8 instr;
    iuab_bytecode_instr_u8_init(instr, op, operand);
    return IUAB_BUFFER_WRITE(compiler->dst, instr);
}

static enum iuab_error
iuab_bytecode_emit_no_operand(struct iuab_bytecode_compiler *compiler) {
    uint8_t op;

    switch (compiler->token.type) {
    case IUAB_TOKEN_BTW: op = IUAB_BYTECODE_OP_WRITE; break;
    case IUAB_TOKEN_BY: op = IUAB_BYTECODE_OP_READ; break;
    case IUAB_TOKEN_GENTOO: op = IUAB_BYTECODE_OP_DEBUG; break;
    default: return IUAB_ERROR_COMPILER_INTERNAL;
    }

    return iuab_buffer_write_u8(compiler->dst, op);
}

static enum iuab_error
iuab_bytecode_begin_loop(struct iuab_bytecode_compiler *compiler) {
    compiler->dst->size += sizeof(iuab_bytecode_instr_size);
    return iuab_buffer_write_size(&compiler->loop_stack, compiler->dst->size);
}

static enum iuab_error
iuab_bytecode_end_loop(struct iuab_bytecode_compiler *compiler) {
    if (compiler->loop_stack.size == 0) {
        return IUAB_ERROR_COMPILER_UNEXPECTED_LOOP_END;
    }

    size_t loop_start = iuab_buffer_pop_size(&compiler->loop_stack);
    iuab_bytecode_instr_size instr;
    iuab_bytecode_instr_size_init(instr, IUAB_BYTECODE_OP_JMPNZ, loop_start);
    enum iuab_error error = IUAB_BUFFER_WRITE(compiler->dst, instr);

    size_t loop_end = compiler->dst->size;
    iuab_bytecode_instr_size_init(instr, IUAB_BYTECODE_OP_JMPZ, loop_end);
    size_t jmpz_instr_offset = loop_start - sizeof(iuab_bytecode_instr_size);
    memcpy(&compiler->dst->data[jmpz_instr_offset], instr, sizeof(instr));

    return error;
}

static enum iuab_error
iuab_bytecode_emit(struct iuab_bytecode_compiler *compiler) {
    enum iuab_error error;

    switch (compiler->token.type) {
    case IUAB_TOKEN_I:
    case IUAB_TOKEN_USE: return iuab_bytecode_emit_additive_p(compiler);
    case IUAB_TOKEN_ARCH:
    case IUAB_TOKEN_LINUX: return iuab_bytecode_emit_additive_v(compiler);
    case IUAB_TOKEN_BTW:
    case IUAB_TOKEN_BY:
    case IUAB_TOKEN_GENTOO:
        error = iuab_bytecode_emit_no_operand(compiler);
        break;
    case IUAB_TOKEN_THE: error = iuab_bytecode_begin_loop(compiler); break;
    case IUAB_TOKEN_WAY: error = iuab_bytecode_end_loop(compiler); break;
    default: return IUAB_ERROR_COMPILER_INVALID_TOKEN;
    }

    if (error == IUAB_ERROR_SUCCESS) {
        compiler->token = iuab_lexer_next_token(&compiler->lexer);
    }

    return error;
}

enum iuab_error iuab_compile_bytecode(
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
) {
    struct iuab_bytecode_compiler compiler;
    enum iuab_error error = iuab_bytecode_compiler_init(&compiler, src, dst);

    if (error != IUAB_ERROR_SUCCESS) {
        *last_token_dst = compiler.token;
        return error;
    }

    while (compiler.token.type != IUAB_TOKEN_EOF) {
        error = iuab_bytecode_emit(&compiler);

        if (error != IUAB_ERROR_SUCCESS) {
            *last_token_dst = compiler.token;
            iuab_bytecode_compiler_fini(&compiler);
            return error;
        }
    }

    *last_token_dst = compiler.token;
    size_t loop_stack_size = compiler.loop_stack.size;
    iuab_bytecode_compiler_fini(&compiler);

    if (loop_stack_size != 0) {
        return IUAB_ERROR_COMPILER_UNCLOSED_LOOPS;
    }

    uint8_t ret[] = { IUAB_BYTECODE_OP_RET };
    return IUAB_BUFFER_WRITE(dst, ret);
}
