// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define C_RESET "\033[0m"
#define C_ERROR "\033[1;31m"
#define C_DEBUG "\033[1;34m"

#define LOG_ERROR(format, ...) \
    fprintf(stderr, C_ERROR "error: " C_RESET format, __VA_ARGS__)

#define LOG_DEBUG(format, ...) \
    printf(C_DEBUG "debug: " C_RESET format, __VA_ARGS__)

#endif // LOG_H
