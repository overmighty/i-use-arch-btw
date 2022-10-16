// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "debug_handler.h"
#include "log.h"

#include "iuab/buffer.h"
#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/targets.h"
#include "iuab/token.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__x86_64__) && defined(IUAB_USE_JIT)
    #define COMPILE_AND_RUN_TARGET IUAB_TARGET_JIT_X86_64
#else
    #define COMPILE_AND_RUN_TARGET IUAB_TARGET_BYTECODE
#endif

int compile(enum iuab_target target, FILE *src, struct iuab_buffer *dst) {
    struct iuab_token last_token;
    enum iuab_error error = iuab_compile(target, src, dst, &last_token);

    if (error != IUAB_ERROR_SUCCESS) {
        LOG_ERROR(
            "compiler error: %s at line %zu, col %zu\n",
            iuab_strerror(error),
            last_token.line,
            last_token.col
        );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int run(enum iuab_target target, const struct iuab_buffer *program) {
    struct iuab_context ctx;
    iuab_context_init(&ctx, program->data, stdin, stdout, debug_handler);
    enum iuab_error error = iuab_run(target, &ctx);

    if (error != IUAB_ERROR_SUCCESS) {
        LOG_ERROR(
            "run-time error: %s at %p (program + %p)\n",
            iuab_strerror(error),
            (void *) ctx.ip,
            (void *) (ctx.ip - ctx.program)
        );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int compile_and_run(const char *filename) {
    FILE *src = fopen(filename, "rbe");

    if (!src) {
        LOG_ERROR("failed to open source file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    enum iuab_target target = COMPILE_AND_RUN_TARGET;
    bool is_jit_target = iuab_target_is_jit(target);

    struct iuab_buffer program;
    enum iuab_error error = iuab_buffer_init_maybe_jit(&program, is_jit_target);

    if (error != IUAB_ERROR_SUCCESS) {
        LOG_ERROR("failed to init program buffer: %s\n", iuab_strerror(error));
        fclose(src);
        return EXIT_FAILURE;
    }

    int status = compile(target, src, &program);
    fclose(src);

    if (status == EXIT_SUCCESS) {
        status = run(target, &program);
    }

    iuab_buffer_fini_maybe_jit(&program, is_jit_target);
    return status;
}
