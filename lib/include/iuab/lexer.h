// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#ifndef IUAB_LEXER_H
#define IUAB_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "token.h"

#include <stdio.h>

// An I use Arch btw source code lexer.
struct iuab_lexer {
    FILE *src;
    size_t line;
    size_t col;
};

// Initializes the given lexer for lexing of the source file pointed to by
// `src`.
void iuab_lexer_init(struct iuab_lexer *lexer, FILE *src);

// Returns the next token from the given lexer.
struct iuab_token iuab_lexer_next_token(struct iuab_lexer *lexer);

#ifdef __cplusplus
};
#endif

#endif // IUAB_LEXER_H
