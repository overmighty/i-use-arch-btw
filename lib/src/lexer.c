// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/lexer.h"

#include "iuab/token.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define IUAB_LEXER_COMMENT_CHAR ';'

static bool iuab_is_space(int ch) {
    return ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ';
}

static bool iuab_is_token_char(int ch) {
    return !iuab_is_space(ch) && ch != IUAB_LEXER_COMMENT_CHAR && ch != EOF;
}

void iuab_lexer_init(struct iuab_lexer *lexer, FILE *src) {
    lexer->src = src;
    lexer->line = 1;
    lexer->col = 0;
}

static int iuab_lexer_next_char(struct iuab_lexer *lexer) {
    lexer->col++;
    // FIXME: on I/O error, propagate IUAB_ERROR_IO to the library consumer.
    return fgetc(lexer->src);
}

static int iuab_lexer_peek_char(struct iuab_lexer *lexer) {
    int ch = fgetc(lexer->src);
    ungetc(ch, lexer->src);
    return ch;
}

static void iuab_lexer_skip_line(struct iuab_lexer *lexer) {
    int ch;

    do {
        ch = iuab_lexer_next_char(lexer);
    } while (ch != '\n' && ch != EOF);

    ungetc('\n', lexer->src);
}

static void iuab_lexer_consume_new_line(struct iuab_lexer *lexer) {
    lexer->line++;
    lexer->col = 0;
}

static enum iuab_token_type iuab_lexer_match_token(
    struct iuab_lexer *lexer,
    const char *end,
    size_t end_len,
    enum iuab_token_type type
) {
    for (size_t i = 0; i < end_len; i++) {
        if (iuab_lexer_next_char(lexer) != end[i]) {
            return IUAB_TOKEN_INVALID;
        }
    }

    if (iuab_is_token_char(iuab_lexer_peek_char(lexer))) {
        return IUAB_TOKEN_INVALID;
    }

    return type;
}

#define IUAB_LEXER_MATCH_TOKEN(lexer, end, type) \
    iuab_lexer_match_token(lexer, end, strlen(end), type)

static enum iuab_token_type
iuab_lexer_next_token_type(struct iuab_lexer *lexer, int ch) {
    switch (ch) {
    case 'i': return IUAB_LEXER_MATCH_TOKEN(lexer, "", IUAB_TOKEN_I);
    case 'u': return IUAB_LEXER_MATCH_TOKEN(lexer, "se", IUAB_TOKEN_USE);
    case 'a': return IUAB_LEXER_MATCH_TOKEN(lexer, "rch", IUAB_TOKEN_ARCH);
    case 'l': return IUAB_LEXER_MATCH_TOKEN(lexer, "inux", IUAB_TOKEN_LINUX);
    case 'b':
        switch (iuab_lexer_next_char(lexer)) {
        case 't': return IUAB_LEXER_MATCH_TOKEN(lexer, "w", IUAB_TOKEN_BTW);
        case 'y': return IUAB_LEXER_MATCH_TOKEN(lexer, "", IUAB_TOKEN_BY);
        default: return IUAB_TOKEN_INVALID;
        }
    case 't': return IUAB_LEXER_MATCH_TOKEN(lexer, "he", IUAB_TOKEN_THE);
    case 'w': return IUAB_LEXER_MATCH_TOKEN(lexer, "ay", IUAB_TOKEN_WAY);
    case 'g': return IUAB_LEXER_MATCH_TOKEN(lexer, "entoo", IUAB_TOKEN_GENTOO);
    default: return IUAB_TOKEN_INVALID;
    }
}

struct iuab_token iuab_lexer_next_token(struct iuab_lexer *lexer) {
    int ch;

    while ((ch = iuab_lexer_next_char(lexer)) != EOF) {
        switch (ch) {
        case IUAB_LEXER_COMMENT_CHAR: iuab_lexer_skip_line(lexer); break;
        case '\n': iuab_lexer_consume_new_line(lexer); break;
        default:
            if (iuab_is_space(ch)) {
                continue;
            }

            size_t col = lexer->col;
            enum iuab_token_type type = iuab_lexer_next_token_type(lexer, ch);
            return (struct iuab_token){ type, lexer->line, col };
        }
    }

    return (struct iuab_token){ IUAB_TOKEN_EOF, lexer->line, lexer->col };
}
