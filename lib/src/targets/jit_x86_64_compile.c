// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/buffer.h"
#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/lexer.h"
#include "iuab/token.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Transforms a 2-byte opcode into a big-endian list of bytes.
#define IUAB_OP2_TO_BYTES(op2) (((op2) >> 8) & 0xFF), ((op2) &0xFF)

// Transforms a dword into a little-endian list of bytes.
#define IUAB_DWORD_TO_BYTES(dword)                                      \
    ((dword) &0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
        (((dword) >> 24) & 0xFF)

// Transforms a qword into a little-endian list of bytes.
#define IUAB_QWORD_TO_BYTES(dword)                                      \
    ((dword) &0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
        (((dword) >> 24) & 0xFF), (((dword) >> 32) & 0xFF),             \
        (((dword) >> 40) & 0xFF), (((dword) >> 48) & 0xFF),             \
        (((dword) >> 56) & 0xFF)

// REX prefixes.
enum {
    IUAB_REX_W = 0x48,
    IUAB_REX_R = 0x44,
    IUAB_REX_B = 0x41,
};

// Instruction primary opcodes.
enum {
    IUAB_OP_ADD_RM8_IMM8 = 0x80 /* /0 ib */,
    IUAB_OP_ADD_RM64_IMM32 = /* REX.W */ 0x81 /* /0 id */,
    IUAB_OP_CALL_REL32 = 0xE8 /* cd */,
    IUAB_OP_CALL_RM64 = 0xFF /* /2 */,
    IUAB_OP_CMP_EAX_IMM32 = 0x3D /* id */,
    IUAB_OP_CMP_RAX_IMM32 = /* REX.W */ 0x3D /* id */,
    IUAB_OP_CMP_RM8_IMM8 = 0x80 /* /7 ib */,
    IUAB_OP_JMP_REL8 = 0xEB /* cb */,
    IUAB_OP_JMP_RM64 = 0xFF /* /4 */,
    IUAB_OP_LEA_R64_M = /* REX.W */ 0x8D /* /r */,
    IUAB_OP_MOV_RM8_R8 = 0x88 /* /r */,
    IUAB_OP_MOV_RM64_R64 = /* REX.W */ 0x89 /* /r */,
    IUAB_OP_MOV_R64_RM64 = /* REX.W */ 0x8B /* /r */,
    IUAB_OP_MOV_R32_IMM32 = 0xB8 /* +rd id */,
    IUAB_OP_MOV_R64_IMM64 = /* REX.W */ 0xB8 /* +rq iq */,
    IUAB_OP_POP_R64 = 0x58 /* +rq */,
    IUAB_OP_POP_RM64 = 0x8F /* /0 */,
    IUAB_OP_PUSH_R64 = 0x50 /* +rq */,
    IUAB_OP_RET_NEAR = 0xC3,
    IUAB_OP_SUB_RM8_IMM8 = 0x80 /* /5 ib */,
    IUAB_OP_SUB_RM64_IMM32 = /* REX.W */ 0x81 /* /5 id */,
    IUAB_OP_SUB_RM64_R64 = /* REX.W */ 0x29 /* /r */,
    IUAB_OP_XOR_RM32_R32 = 0x31 /* /r */,

    IUAB_OP2_JAE_REL32 = 0x0F83 /* cd */,
    IUAB_OP2_JB_REL32 = 0x0F82 /* cd */,
    IUAB_OP2_JE_REL32 = 0x0F84 /* cd */,
    IUAB_OP2_JNE_REL8 = 0x0F75 /* cb */,
    IUAB_OP2_JNE_REL32 = 0x0F85 /* cd */,
    IUAB_OP2_MOVZX_R32_RM8 = 0x0FB6 /* /r */,
};

// Register IDs.
enum {
    IUAB_REG_AL = 0x0,

    IUAB_REG_EAX = 0x0,
    IUAB_REG_EDI = 0x7,

    IUAB_REG_RAX = 0x0,
    IUAB_REG_RBX = 0x3,
    IUAB_REG_RSI = 0x6,
    IUAB_REG_RDI = 0x7,
    IUAB_REG_R12 = 0x4,
    IUAB_REG_R13 = 0x5,
    IUAB_REG_R14 = 0x6,
    IUAB_REG_R15 = 0x7,
};

// ModR/M byte `mod` field values.
enum {
    IUAB_MODRM_MOD_DISP0 = 0x0 << 6,
    IUAB_MODRM_MOD_DISP8 = 0x1 << 6,
    IUAB_MODRM_MOD_DIRECT = 0x3 << 6,
};

// ModR/M byte `reg` field values.
enum {
    IUAB_MODRM_REG_OP_ADD_RM_IMM = 0x0 << 3,
    IUAB_MODRM_REG_OP_CALL_RM = 0x2 << 3,
    IUAB_MODRM_REG_OP_CMP_RM_IMM = 0x7 << 3,
    IUAB_MODRM_REG_OP_SUB_RM_IMM = 0x5 << 3,
    IUAB_MODRM_REG_OP_JMP_RM = 0x4 << 3,
    IUAB_MODRM_REG_OP_POP_RM = 0x0 << 3,

    IUAB_MODRM_REG_AL = IUAB_REG_AL << 3,

    IUAB_MODRM_REG_EAX = IUAB_REG_EAX << 3,
    IUAB_MODRM_REG_EDI = IUAB_REG_EDI << 3,

    IUAB_MODRM_REG_RBX = IUAB_REG_RBX << 3,
    IUAB_MODRM_REG_RSI = IUAB_REG_RSI << 3,
    IUAB_MODRM_REG_RDI = IUAB_REG_RDI << 3,
    IUAB_MODRM_REG_R14 = IUAB_REG_R14 << 3,
    IUAB_MODRM_REG_R15 = IUAB_REG_R15 << 3,
};

// ModR/M byte `rm` field values.
enum {
    IUAB_MODRM_RM_EAX = IUAB_REG_EAX,
    IUAB_MODRM_RM_RAX = IUAB_REG_RAX,
    IUAB_MODRM_RM_RBX = IUAB_REG_RBX,
    IUAB_MODRM_RM_RDI = IUAB_REG_RDI,
    IUAB_MODRM_RM_R12 = IUAB_REG_R12,
    IUAB_MODRM_RM_R13 = IUAB_REG_R13,
    IUAB_MODRM_RM_R14 = IUAB_REG_R14,
};

enum iuab_jit_x86_64_jump_target {
    IUAB_JUMP_RET_ERROR_DP_OUT_OF_BOUNDS,
    IUAB_JUMP_RET_ERROR_IO,
    IUAB_JUMP_HANDLE_FGETC_EOF,
    IUAB_JUMP_CALL_DEBUG_HANDLER,

    IUAB_NUM_JUMP_TARGETS,
};

struct iuab_jit_x86_64_jump {
    size_t from;
    enum iuab_jit_x86_64_jump_target to;
};

struct iuab_jit_x86_64_compiler {
    struct iuab_lexer lexer;
    struct iuab_token token;
    struct iuab_buffer jumps;
    struct iuab_buffer loop_stack;
    struct iuab_buffer *dst;
};

static enum iuab_error iuab_jit_x86_64_compiler_init(
    struct iuab_jit_x86_64_compiler *compiler,
    FILE *src,
    struct iuab_buffer *dst
) {
    compiler->dst = dst;
    iuab_lexer_init(&compiler->lexer, src);
    compiler->token = iuab_lexer_next_token(&compiler->lexer);
    enum iuab_error error = iuab_buffer_init(&compiler->jumps);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_buffer_init(&compiler->loop_stack);
}

static void
iuab_jit_x86_64_compiler_fini(struct iuab_jit_x86_64_compiler *compiler) {
    iuab_buffer_fini(&compiler->jumps);
    iuab_buffer_fini(&compiler->loop_stack);
}

static enum iuab_error iuab_jit_x86_64_emit_header(struct iuab_buffer *dst) {
    uint8_t instrs[] = {
        // push rbx
        IUAB_OP_PUSH_R64 + IUAB_REG_RBX,
        // push r12
        IUAB_REX_B,
        IUAB_OP_PUSH_R64 + IUAB_REG_R12,
        // push r13
        IUAB_REX_B,
        IUAB_OP_PUSH_R64 + IUAB_REG_R13,
        // push r14
        IUAB_REX_B,
        IUAB_OP_PUSH_R64 + IUAB_REG_R14,
        // push r15
        IUAB_REX_B,
        IUAB_OP_PUSH_R64 + IUAB_REG_R15,
        // mov rbx, rdi
        IUAB_REX_W,
        IUAB_OP_MOV_RM64_R64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_RDI | IUAB_MODRM_RM_RBX,
        // mov r12, fgetc
        IUAB_REX_W | IUAB_REX_B,
        IUAB_OP_MOV_R64_IMM64 + IUAB_REG_R12,
        IUAB_QWORD_TO_BYTES((int64_t) fgetc),
        // mov r13, fputc
        IUAB_REX_W | IUAB_REX_B,
        IUAB_OP_MOV_R64_IMM64 + IUAB_REG_R13,
        IUAB_QWORD_TO_BYTES((int64_t) fputc),
        // mov r14, QWORD PTR [rdi + offsetof(struct iuab_context, dp)]
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_MOV_R64_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_R14 | IUAB_MODRM_RM_RDI,
        offsetof(struct iuab_context, dp),
        // lea r15, QWORD PTR [rdi + offsetof(struct iuab_context, memory)]
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_LEA_R64_M,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_R15 | IUAB_MODRM_RM_RDI,
        offsetof(struct iuab_context, memory),
    };
    return IUAB_BUFFER_WRITE_JIT(dst, instrs);
}

static enum iuab_error
iuab_jit_x86_64_set_rel8(struct iuab_buffer *dst, size_t from, size_t to) {
    ssize_t rel8 = (ssize_t) (to - from);

    if (rel8 < INT8_MIN || rel8 > INT8_MAX) {
        return IUAB_ERROR_JIT_JUMP_TOO_LARGE;
    }

    *(int8_t *) &dst->data[from - sizeof(int8_t)] = (int8_t) rel8;
    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error
iuab_jit_x86_64_set_rel32(struct iuab_buffer *dst, size_t from, size_t to) {
    ssize_t rel32 = (ssize_t) (to - from);

    if (rel32 < INT32_MIN || rel32 > INT32_MAX) {
        return IUAB_ERROR_JIT_JUMP_TOO_LARGE;
    }

    uint8_t *rel32_dst = &dst->data[from - sizeof(int32_t)];
    rel32_dst[0] = rel32 & 0xFF;
    rel32_dst[1] = (rel32 >> 8) & 0xFF;
    rel32_dst[2] = (rel32 >> 16) & 0xFF;
    rel32_dst[3] = (rel32 >> 24) & 0xFF;
    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error iuab_jit_x86_64_emit_ret_error_dp_out_of_bounds(
    size_t exit_offset,
    struct iuab_buffer *dst
) {
    uint8_t instrs[] = {
        // mov eax, IUAB_ERROR_DP_OUT_OF_BOUNDS
        IUAB_OP_MOV_R32_IMM32 + IUAB_REG_EAX,
        IUAB_DWORD_TO_BYTES(IUAB_ERROR_DP_OUT_OF_BOUNDS),
        // jmp .exit ; Offset written later.
        IUAB_OP_JMP_REL8,
        0,
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(dst, instrs);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_jit_x86_64_set_rel8(dst, dst->size, exit_offset);
}

static enum iuab_error
iuab_jit_x86_64_emit_ret_error_io(size_t exit_offset, struct iuab_buffer *dst) {
    uint8_t instrs[] = {
        // mov eax, IUAB_ERROR_IO
        IUAB_OP_MOV_R32_IMM32 + IUAB_REG_EAX,
        IUAB_DWORD_TO_BYTES(IUAB_ERROR_IO),
        // jmp .exit ; Offset written later.
        IUAB_OP_JMP_REL8,
        0,
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(dst, instrs);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_jit_x86_64_set_rel8(dst, dst->size, exit_offset);
}

static enum iuab_error iuab_jit_x86_64_emit_handle_fgetc_error(
    size_t exit_offset,
    struct iuab_buffer *dst
) {
    uint8_t ret_error_io_if_ferror[] = {
        // mov rdi, QWORD PTR [rbx + offsetof(struct iuab_context, in)]
        IUAB_REX_W,
        IUAB_OP_MOV_R64_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_RDI | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, in),
        // mov rax, ferror
        IUAB_REX_W,
        IUAB_OP_MOV_R64_IMM64 + IUAB_REG_EAX,
        IUAB_QWORD_TO_BYTES((int64_t) ferror),
        // call rax
        IUAB_OP_CALL_RM64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_OP_CALL_RM | IUAB_MODRM_RM_RAX,
        // cmp eax, 0
        IUAB_OP_CMP_EAX_IMM32,
        IUAB_DWORD_TO_BYTES(0),
        // mov eax, IUAB_ERROR_IO
        IUAB_OP_MOV_R32_IMM32 + IUAB_REG_EAX,
        IUAB_DWORD_TO_BYTES(IUAB_ERROR_IO),
        // jne .exit ; Offset written later.
        IUAB_OP2_TO_BYTES(IUAB_OP2_JNE_REL8),
        0,
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(dst, ret_error_io_if_ferror);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    error = iuab_jit_x86_64_set_rel8(dst, dst->size, exit_offset);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    uint8_t ret_error_runtime_end_of_input_file[] = {
        // mov eax, IUAB_ERROR_RUNTIME_END_OF_INPUT_FILE
        IUAB_OP_MOV_R32_IMM32 + IUAB_REG_EAX,
        IUAB_DWORD_TO_BYTES(IUAB_ERROR_RUNTIME_END_OF_INPUT_FILE),
        // jmp .exit
        IUAB_OP_JMP_REL8,
        0,
    };
    error = IUAB_BUFFER_WRITE_JIT(dst, ret_error_runtime_end_of_input_file);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_jit_x86_64_set_rel8(dst, dst->size, exit_offset);
}

static enum iuab_error
iuab_jit_x86_64_emit_call_debug_handler(struct iuab_buffer *dst) {
    uint8_t instrs[] = {
        // pop QWORD PTR [rbx]
        IUAB_REX_W,
        IUAB_OP_POP_RM64,
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_OP_POP_RM | IUAB_MODRM_RM_RBX,
        // mov QWORD PTR [rbx + offsetof(struct iuab_context, dp)], r14
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_MOV_RM64_R64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_R14 | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, dp),
        // mov rdi, rbx
        IUAB_REX_W,
        IUAB_OP_MOV_RM64_R64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_RBX | IUAB_MODRM_RM_RDI,
        // call QWORD PTR [rbx + offsetof(struct iuab_context, debug_handler)]
        IUAB_OP_CALL_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_OP_CALL_RM | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, debug_handler),
        // mov r14, QWORD PTR [rbx + offsetof(struct iuab_context, dp)]
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_MOV_R64_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_R14 | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, dp),
        // jmp QWORD PTR [rbx]
        IUAB_OP_JMP_RM64,
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_OP_JMP_RM | IUAB_MODRM_RM_RBX,
    };
    return IUAB_BUFFER_WRITE_JIT(dst, instrs);
}

static enum iuab_error iuab_jit_x86_64_emit_jump_target(
    enum iuab_jit_x86_64_jump_target target,
    size_t exit_offset,
    struct iuab_buffer *dst
) {
    switch (target) {
    case IUAB_JUMP_RET_ERROR_DP_OUT_OF_BOUNDS:
        return iuab_jit_x86_64_emit_ret_error_dp_out_of_bounds(
            exit_offset,
            dst
        );
    case IUAB_JUMP_RET_ERROR_IO:
        return iuab_jit_x86_64_emit_ret_error_io(exit_offset, dst);
    case IUAB_JUMP_HANDLE_FGETC_EOF:
        return iuab_jit_x86_64_emit_handle_fgetc_error(exit_offset, dst);
    case IUAB_JUMP_CALL_DEBUG_HANDLER:
        return iuab_jit_x86_64_emit_call_debug_handler(dst);
    default: return IUAB_ERROR_COMPILER_INTERNAL;
    }
}

static enum iuab_error iuab_jit_x86_64_emit_footer(
    struct iuab_buffer *jumps,
    struct iuab_buffer *dst
) {
    uint8_t set_error_success[] = {
        // xor eax, eax
        IUAB_OP_XOR_RM32_R32,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_EAX | IUAB_MODRM_RM_EAX,
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(dst, set_error_success);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    size_t exit_offset = dst->size;
    uint8_t exit[] = {
        // pop r15
        IUAB_REX_B,
        IUAB_OP_POP_R64 + IUAB_REG_R15,
        // pop r14
        IUAB_REX_B,
        IUAB_OP_POP_R64 + IUAB_REG_R14,
        // pop r13
        IUAB_REX_B,
        IUAB_OP_POP_R64 + IUAB_REG_R13,
        // pop r12
        IUAB_REX_B,
        IUAB_OP_POP_R64 + IUAB_REG_R12,
        // pop rbx
        IUAB_OP_POP_R64 + IUAB_REG_RBX,
        // ret
        IUAB_OP_RET_NEAR,
    };
    error = IUAB_BUFFER_WRITE_JIT(dst, exit);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    size_t jump_target_offsets[IUAB_NUM_JUMP_TARGETS] = { 0 };
    size_t jump_struct_size = sizeof(struct iuab_jit_x86_64_jump);

    for (size_t i = 0; i < jumps->size; i += jump_struct_size) {
        struct iuab_jit_x86_64_jump *jump =
            (struct iuab_jit_x86_64_jump *) &jumps->data[i];

        if (jump_target_offsets[jump->to] == 0) {
            jump_target_offsets[jump->to] = dst->size;
            iuab_jit_x86_64_emit_jump_target(jump->to, exit_offset, dst);
        }

        error = iuab_jit_x86_64_set_rel32(
            dst,
            jump->from,
            jump_target_offsets[jump->to]
        );

        if (error != IUAB_ERROR_SUCCESS) {
            return error;
        }
    }

    return IUAB_ERROR_SUCCESS;
}

static enum iuab_error
iuab_jit_x86_64_emit_additive_p(struct iuab_jit_x86_64_compiler *compiler) {
    struct iuab_lexer *lexer = &compiler->lexer;
    enum iuab_token_type token_type = compiler->token.type;

    uint16_t operand = 1;
    struct iuab_token next_token;

    while ((next_token = iuab_lexer_next_token(lexer)).type == token_type) {
        if (operand == UINT16_MAX) {
            compiler->token = next_token;
            return IUAB_ERROR_DP_OUT_OF_BOUNDS;
        }

        operand++;
    }

    uint16_t jcc_op;
    uint8_t op;
    uint8_t modrm_reg;
    int32_t cmp_bound;

    switch (token_type) {
    case IUAB_TOKEN_I:
        jcc_op = IUAB_OP2_JAE_REL32;
        op = IUAB_OP_ADD_RM64_IMM32;
        modrm_reg = IUAB_MODRM_REG_OP_ADD_RM_IMM;
        cmp_bound = IUAB_CONTEXT_MEMORY_SIZE - operand;
        break;
    case IUAB_TOKEN_USE:
        jcc_op = IUAB_OP2_JB_REL32;
        op = IUAB_OP_SUB_RM64_IMM32;
        modrm_reg = IUAB_MODRM_REG_OP_SUB_RM_IMM;
        cmp_bound = operand;
        break;
    default: return IUAB_ERROR_COMPILER_INTERNAL;
    }

    compiler->token = next_token;

    uint8_t bounds_check[] = {
        // mov rax, r14
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_MOV_RM64_R64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_R14 | IUAB_MODRM_RM_RAX,
        // sub rax, r15
        IUAB_REX_W | IUAB_REX_R,
        IUAB_OP_SUB_RM64_R64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_R15 | IUAB_MODRM_RM_RAX,
        // cmp rax, cmp_bound
        IUAB_REX_W,
        IUAB_OP_CMP_RAX_IMM32,
        IUAB_DWORD_TO_BYTES(cmp_bound),
        // (jae|jb) .ret_dp_out_of_bounds ; Offset written later.
        IUAB_OP2_TO_BYTES(jcc_op),
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, bounds_check);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    struct iuab_jit_x86_64_jump jump_on_error = {
        .from = compiler->dst->size,
        .to = IUAB_JUMP_RET_ERROR_DP_OUT_OF_BOUNDS,
    };
    error = iuab_buffer_write(
        &compiler->jumps,
        &jump_on_error,
        sizeof(jump_on_error)
    );

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    uint8_t instr[] = {
        // (add|sub) r14, operand
        IUAB_REX_W | IUAB_REX_B,
        op,
        IUAB_MODRM_MOD_DIRECT | modrm_reg | IUAB_MODRM_RM_R14,
        IUAB_DWORD_TO_BYTES(operand),
    };
    return IUAB_BUFFER_WRITE_JIT(compiler->dst, instr);
}

static enum iuab_error
iuab_jit_x86_64_emit_additive_v(struct iuab_jit_x86_64_compiler *compiler) {
    enum iuab_token_type token_type = compiler->token.type;
    struct iuab_lexer *lexer = &compiler->lexer;

    uint8_t op;
    uint8_t modrm_reg;

    switch (token_type) {
    case IUAB_TOKEN_ARCH:
        op = IUAB_OP_ADD_RM8_IMM8;
        modrm_reg = IUAB_MODRM_REG_OP_ADD_RM_IMM;
        break;
    case IUAB_TOKEN_LINUX:
        op = IUAB_OP_SUB_RM8_IMM8;
        modrm_reg = IUAB_MODRM_REG_OP_SUB_RM_IMM;
        break;
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

    uint8_t instr[] = {
        // (add|sub) BYTE PTR [r14], operand
        IUAB_REX_B,
        op,
        IUAB_MODRM_MOD_DISP0 | modrm_reg | IUAB_MODRM_RM_R14,
        operand,
    };
    return IUAB_BUFFER_WRITE_JIT(compiler->dst, instr);
}

static enum iuab_error
iuab_jit_x86_64_emit_write(struct iuab_jit_x86_64_compiler *compiler) {
    uint8_t instrs[] = {
        // movzx edi, BYTE PTR [r14]
        IUAB_REX_B,
        IUAB_OP2_TO_BYTES(IUAB_OP2_MOVZX_R32_RM8),
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_EDI | IUAB_MODRM_RM_R14,
        // mov rsi, QWORD PTR [rbx + offsetof(struct iuab_context, out)]
        IUAB_REX_W,
        IUAB_OP_MOV_R64_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_RSI | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, out),
        // call r13
        IUAB_REX_B,
        IUAB_OP_CALL_RM64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_OP_CALL_RM | IUAB_MODRM_RM_R13,
        // cmp eax, -1
        IUAB_OP_CMP_EAX_IMM32,
        IUAB_DWORD_TO_BYTES((int32_t) -1),
        // je .ret_error_io ; Offset written later.
        IUAB_OP2_TO_BYTES(IUAB_OP2_JE_REL32),
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, instrs);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    struct iuab_jit_x86_64_jump jump = {
        .from = compiler->dst->size,
        .to = IUAB_JUMP_RET_ERROR_IO,
    };
    return iuab_buffer_write(&compiler->jumps, &jump, sizeof(jump));
}

static enum iuab_error
iuab_jit_x86_64_emit_read(struct iuab_jit_x86_64_compiler *compiler) {
    uint8_t call_fgetc[] = {
        // mov rdi, QWORD PTR [rbx + offsetof(struct iuab_context, in)]
        IUAB_REX_W,
        IUAB_OP_MOV_R64_RM64,
        IUAB_MODRM_MOD_DISP8 | IUAB_MODRM_REG_RDI | IUAB_MODRM_RM_RBX,
        offsetof(struct iuab_context, in),
        // call r12
        IUAB_REX_B,
        IUAB_OP_CALL_RM64,
        IUAB_MODRM_MOD_DIRECT | IUAB_MODRM_REG_OP_CALL_RM | IUAB_MODRM_RM_R12,
        // cmp eax, -1
        IUAB_OP_CMP_EAX_IMM32,
        IUAB_DWORD_TO_BYTES((int32_t) -1),
        // je .handle_fgetc_eof ; Offset written later.
        IUAB_OP2_TO_BYTES(IUAB_OP2_JE_REL32),
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, call_fgetc);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    struct iuab_jit_x86_64_jump jump = {
        .from = compiler->dst->size,
        .to = IUAB_JUMP_HANDLE_FGETC_EOF,
    };
    error = iuab_buffer_write(&compiler->jumps, &jump, sizeof(jump));

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    uint8_t write_returned_char_to_memory[] = {
        // mov BYTE PTR [r14], al
        IUAB_REX_B,
        IUAB_OP_MOV_RM8_R8,
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_AL | IUAB_MODRM_RM_R14,
    };
    return IUAB_BUFFER_WRITE_JIT(compiler->dst, write_returned_char_to_memory);
}

static enum iuab_error
iuab_jit_x86_64_begin_loop(struct iuab_jit_x86_64_compiler *compiler) {
    uint8_t instrs[] = {
        // cmp BYTE PTR [r14], 0
        IUAB_REX_B,
        IUAB_OP_CMP_RM8_IMM8,
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_OP_CMP_RM_IMM | IUAB_MODRM_RM_R14,
        0,
        // je loop_end ; Offset written later.
        IUAB_OP2_TO_BYTES(IUAB_OP2_JE_REL32),
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, instrs);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_buffer_write_size(&compiler->loop_stack, compiler->dst->size);
}

static enum iuab_error
iuab_jit_x86_64_end_loop(struct iuab_jit_x86_64_compiler *compiler) {
    if (compiler->loop_stack.size == 0) {
        return IUAB_ERROR_COMPILER_UNEXPECTED_LOOP_END;
    }

    uint8_t instrs[] = {
        // cmp BYTE PTR [r14], 0
        IUAB_REX_B,
        IUAB_OP_CMP_RM8_IMM8,
        IUAB_MODRM_MOD_DISP0 | IUAB_MODRM_REG_OP_CMP_RM_IMM | IUAB_MODRM_RM_R14,
        0,
        // jne loop_start ; Offset written later.
        IUAB_OP2_TO_BYTES(IUAB_OP2_JNE_REL32),
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, instrs);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    size_t loop_start = iuab_buffer_pop_size(&compiler->loop_stack);
    error = iuab_jit_x86_64_set_rel32(
        compiler->dst,
        compiler->dst->size,
        loop_start
    );

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    return iuab_jit_x86_64_set_rel32(
        compiler->dst,
        loop_start,
        compiler->dst->size
    );
}

static enum iuab_error
iuab_jit_x86_64_emit_debug(struct iuab_jit_x86_64_compiler *compiler) {
    uint8_t instr[] = {
        // call .call_debug_handler ; Offset written later.
        IUAB_OP_CALL_REL32,
        IUAB_DWORD_TO_BYTES(0),
    };
    enum iuab_error error = IUAB_BUFFER_WRITE_JIT(compiler->dst, instr);

    if (error != IUAB_ERROR_SUCCESS) {
        return error;
    }

    struct iuab_jit_x86_64_jump jump = {
        .from = compiler->dst->size,
        .to = IUAB_JUMP_CALL_DEBUG_HANDLER,
    };
    return iuab_buffer_write(&compiler->jumps, &jump, sizeof(jump));
}

static enum iuab_error
iuab_jit_x86_64_emit(struct iuab_jit_x86_64_compiler *compiler) {
    enum iuab_error error;

    switch (compiler->token.type) {
    case IUAB_TOKEN_I:
    case IUAB_TOKEN_USE: return iuab_jit_x86_64_emit_additive_p(compiler);
    case IUAB_TOKEN_ARCH:
    case IUAB_TOKEN_LINUX: return iuab_jit_x86_64_emit_additive_v(compiler);
    case IUAB_TOKEN_BTW: error = iuab_jit_x86_64_emit_write(compiler); break;
    case IUAB_TOKEN_BY: error = iuab_jit_x86_64_emit_read(compiler); break;
    case IUAB_TOKEN_THE: error = iuab_jit_x86_64_begin_loop(compiler); break;
    case IUAB_TOKEN_WAY: error = iuab_jit_x86_64_end_loop(compiler); break;
    case IUAB_TOKEN_GENTOO: error = iuab_jit_x86_64_emit_debug(compiler); break;
    default: return IUAB_ERROR_COMPILER_INVALID_TOKEN;
    }

    if (error == IUAB_ERROR_SUCCESS) {
        compiler->token = iuab_lexer_next_token(&compiler->lexer);
    }

    return error;
}

enum iuab_error iuab_compile_jit_x86_64(
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
) {
    struct iuab_jit_x86_64_compiler compiler;
    enum iuab_error error = iuab_jit_x86_64_compiler_init(&compiler, src, dst);

    if (error != IUAB_ERROR_SUCCESS) {
        *last_token_dst = compiler.token;
        return error;
    }

    error = iuab_jit_x86_64_emit_header(dst);

    if (error != IUAB_ERROR_SUCCESS) {
        *last_token_dst = compiler.token;
        iuab_jit_x86_64_compiler_fini(&compiler);
        return error;
    }

    while (compiler.token.type != IUAB_TOKEN_EOF) {
        error = iuab_jit_x86_64_emit(&compiler);

        if (error != IUAB_ERROR_SUCCESS) {
            *last_token_dst = compiler.token;
            iuab_jit_x86_64_compiler_fini(&compiler);
            return error;
        }
    }

    *last_token_dst = compiler.token;

    if (compiler.loop_stack.size != 0) {
        iuab_jit_x86_64_compiler_fini(&compiler);
        return IUAB_ERROR_COMPILER_UNCLOSED_LOOPS;
    }

    error = iuab_jit_x86_64_emit_footer(&compiler.jumps, dst);
    iuab_jit_x86_64_compiler_fini(&compiler);
    return error;
}
