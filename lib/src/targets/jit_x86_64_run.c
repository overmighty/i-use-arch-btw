// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/context.h"
#include "iuab/errors.h"

enum iuab_error iuab_run_jit_x86_64(struct iuab_context *ctx) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    return ((enum iuab_error(*)(struct iuab_context *)) ctx->ip)(ctx);
#pragma GCC diagnostic pop
}
