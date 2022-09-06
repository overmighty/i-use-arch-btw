// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_ERRORS_H
#define IUAB_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

// I use Arch btw errors.
enum iuab_error {
    // Success.
    IUAB_ERROR_SUCCESS,

    // Memory allocation error.
    IUAB_ERROR_MALLOC,
    // Input/output error.
    IUAB_ERROR_IO,

    // Invalid target.
    IUAB_ERROR_INVALID_TARGET,

    // Invalid token.
    IUAB_ERROR_COMPILER_INVALID_TOKEN,
    // Unexpected loop end.
    IUAB_ERROR_COMPILER_UNEXPECTED_LOOP_END,
    // Unclosed loops.
    IUAB_ERROR_COMPILER_UNCLOSED_LOOPS,
    // Internal compiler error.
    IUAB_ERROR_COMPILER_INTERNAL,

    // Data pointer out of bounds.
    IUAB_ERROR_DP_OUT_OF_BOUNDS,

    // End of input file.
    IUAB_ERROR_RUNTIME_END_OF_INPUT_FILE,

    // Invalid bytecode opcode.
    IUAB_ERROR_BYTECODE_INVALID_OP,
};

// Returns a description of the given error as a string.
const char *iuab_strerror(enum iuab_error error);

#ifdef __cplusplus
};
#endif

#endif // IUAB_ERRORS_H
