#ifndef IUAB_LEXER_H
#define IUAB_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "token.h"

#include <stdio.h>

/* A lexer for "I use Arch btw" source files. */
struct iuab_lexer {
    FILE *input; /* The input source file. */
    int ch; /* The current character. */
    size_t line; /* The current character's line number. */
    size_t col; /* The current character's column number. */
};

/* Initializes the given lexer for lexing of the given input file. */
void iuab_lexer_init(struct iuab_lexer *lexer, FILE *input);

/* Lexes the next token of the given lexer's input and returns it. */
struct iuab_token iuab_lexer_next_token(struct iuab_lexer *lexer);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_LEXER_H */
