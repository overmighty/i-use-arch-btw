// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/errors.h"

const char *iuab_strerror(enum iuab_error error) {
    switch (error) {
    case IUAB_ERROR_SUCCESS: return "success";
    case IUAB_ERROR_MALLOC: return "memory allocation error";
    case IUAB_ERROR_IO: return "input/output error";
    case IUAB_ERROR_COMPILER_INVALID_TOKEN: return "invalid token";
    case IUAB_ERROR_COMPILER_UNEXPECTED_LOOP_END: return "unexpected loop end";
    case IUAB_ERROR_COMPILER_UNCLOSED_LOOPS: return "unclosed loops";
    case IUAB_ERROR_COMPILER_INTERNAL: return "internal compiler error";
    case IUAB_ERROR_DP_OUT_OF_BOUNDS: return "data pointer out of bounds";
    case IUAB_ERROR_RUNTIME_END_OF_INPUT_FILE: return "end of input file";
    case IUAB_ERROR_BYTECODE_INVALID_OP: return "invalid bytecode opcode";
    case IUAB_ERROR_JIT_JUMP_TOO_LARGE: return "jump too large";
    default: return "???";
    }
}
