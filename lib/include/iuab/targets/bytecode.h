// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_TARGETS_BYTECODE_H
#define IUAB_TARGETS_BYTECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../buffer.h"
#include "../context.h"
#include "../errors.h"
#include "../token.h"

#include <stdint.h>
#include <stdio.h>

// I use Arch btw bytecode opcodes.
enum {
    // Return from the program.
    IUAB_BYTECODE_OP_RET,
    // Add to the data pointer the following `uint16_t` value.
    IUAB_BYTECODE_OP_ADDP,
    // Subtract from the data pointer the following `uint16_t` value.
    IUAB_BYTECODE_OP_SUBP,
    // Add to the value pointed to by the data pointer the following `uint8_t`
    // value.
    IUAB_BYTECODE_OP_ADDV,
    // Subtract from the value pointed to by the data pointer the following
    // `uint8_t` value.
    IUAB_BYTECODE_OP_SUBV,
    // Write the value pointed to by the data pointer as character to the output
    // file.
    IUAB_BYTECODE_OP_WRITE,
    // Read a character from the input file into the value pointed to by the
    // data pointer.
    IUAB_BYTECODE_OP_READ,
    // Jump if the value pointed to by the data pointer is zero to the following
    // `size_t` program offset.
    IUAB_BYTECODE_OP_JMPZ,
    // Jump if the value pointed to by the data pointer is not zero to the
    // following `size_t` program offset.
    IUAB_BYTECODE_OP_JMPNZ,
    // Call the debugging event handler.
    IUAB_BYTECODE_OP_DEBUG,
};

// Returns the name of the given I use Arch btw bytecode opcode as a string.
const char *iuab_bytecode_op_name(uint8_t op);

// Compiles the source file pointed to by `src` into I use Arch btw bytecode to
// write to the buffer pointed to by `dst` and writes the last token
// processed at the location pointed to by `last_token_dst`. Returns the error
// that occurred in the process.
enum iuab_error iuab_compile_bytecode(
    FILE *src,
    struct iuab_buffer *dst,
    struct iuab_token *last_token_dst
);

// Runs the I use Arch btw bytecode program from the context pointed to by
// `ctx`. Returns the error that occurred in the process.
enum iuab_error iuab_run_bytecode(struct iuab_context *ctx);

#ifdef __cplusplus
}
#endif

#endif // IUAB_TARGETS_BYTECODE_H
