// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_TOKEN_H
#define IUAB_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

// A type of I use Arch btw source code token.
enum iuab_token_type {
    // End of file.
    IUAB_TOKEN_EOF = -1,

    // The `i` keyword.
    IUAB_TOKEN_I,
    // The `use` keyword.
    IUAB_TOKEN_USE,
    // The `arch` keyword.
    IUAB_TOKEN_ARCH,
    // The `linux` keyword.
    IUAB_TOKEN_LINUX,
    // The `btw` keyword.
    IUAB_TOKEN_BTW,
    // The `by` keyword.
    IUAB_TOKEN_BY,
    // The `the` keyword.
    IUAB_TOKEN_THE,
    // The `way` keyword.
    IUAB_TOKEN_WAY,
    // The `gentoo` keyword.
    IUAB_TOKEN_GENTOO,

    // Invalid token type.
    IUAB_TOKEN_INVALID
};

// Returns the name of the given token type as a string.
const char *iuab_token_type_name(enum iuab_token_type type);

// An I use Arch btw source code token.
struct iuab_token {
    enum iuab_token_type type;
    size_t line;
    size_t col;
};

#ifdef __cplusplus
}
#endif

#endif
