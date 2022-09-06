// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "debug_handler.h"

#include "log.h"

#include "iuab/context.h"

#include <stdint.h>

#define C_OP "\033[96m"
#define C_COMMENT "\033[90m"
#define C_BASE_VAR "\033[37m"
#define C_NUMERIC "\033[33m"

#define DEBUG_FORMAT_REL_PTR(name, base_name)                             \
    name " " C_OP "= " C_NUMERIC "%p " C_COMMENT "(" C_BASE_VAR base_name \
         " " C_OP "+ " C_NUMERIC "%p" C_COMMENT ")"

#define DEBUG_FORMAT_DEREF_PTR(name, value_format) \
    C_OP "*" C_RESET name " " C_OP "= " C_NUMERIC value_format C_RESET

#define DEBUG_FORMAT_AND C_OP "; " C_RESET

void debug_handler(struct iuab_context *ctx) {
    LOG_DEBUG(
        DEBUG_FORMAT_REL_PTR("ip", "program")
            DEBUG_FORMAT_AND DEBUG_FORMAT_REL_PTR("dp", "memory")
                DEBUG_FORMAT_AND DEBUG_FORMAT_DEREF_PTR("dp", "%hhu") "\n",
        (void *) ctx->ip,
        (void *) (ctx->ip - ctx->program),
        (void *) ctx->dp,
        (void *) (ctx->dp - ctx->memory),
        *ctx->dp
    );
}
