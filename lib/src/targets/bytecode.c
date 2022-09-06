// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/targets/bytecode.h"

#include <stdint.h>

const char *iuab_bytecode_op_name(uint8_t op) {
    switch (op) {
    case IUAB_BYTECODE_OP_RET: return "ret";
    case IUAB_BYTECODE_OP_ADDP: return "addp";
    case IUAB_BYTECODE_OP_SUBP: return "subp";
    case IUAB_BYTECODE_OP_ADDV: return "addv";
    case IUAB_BYTECODE_OP_SUBV: return "subv";
    case IUAB_BYTECODE_OP_WRITE: return "write";
    case IUAB_BYTECODE_OP_READ: return "read";
    case IUAB_BYTECODE_OP_JMPZ: return "jmpz";
    case IUAB_BYTECODE_OP_JMPNZ: return "jmpnz";
    case IUAB_BYTECODE_OP_DEBUG: return "debug";
    default: return "???";
    }
}
