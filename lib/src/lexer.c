#include <iuab/lexer.h>

#include <ctype.h> /* for isspace */

#define COMMENT_CHAR ';'

#define IS_TOKEN_CHAR(c) (!isspace(c) && (c) != EOF)

void iuab_lexer_init(struct iuab_lexer *lexer, FILE *input) {
    lexer->input = input;
    lexer->ch = fgetc(lexer->input);
    lexer->line = 1;
    lexer->col = 1;
}

static int iuab_lexer_next_char(struct iuab_lexer *lexer) {
    lexer->col++;
    lexer->ch = fgetc(lexer->input);
    return lexer->ch;
}

static void iuab_lexer_new_line(struct iuab_lexer *lexer) {
    lexer->ch = fgetc(lexer->input);
    lexer->line++;
    lexer->col = 1;
}

static void iuab_lexer_skip_comment(struct iuab_lexer *lexer) {
    do {
        iuab_lexer_next_char(lexer);
    } while (lexer->ch != '\n' && lexer->ch != EOF);
}

static enum iuab_token_type iuab_lexer_match_end(struct iuab_lexer *lexer,
                                                 const char *end,
                                                 enum iuab_token_type type) {
    for (int i = 0; end[i] != '\0'; i++) {
        if (iuab_lexer_next_char(lexer) != end[i]) {
            return IUAB_TOKEN_INVAL;
        }
    }

    iuab_lexer_next_char(lexer);

    if (IS_TOKEN_CHAR(lexer->ch)) {
        return IUAB_TOKEN_INVAL;
    }

    return type;
}

static enum iuab_token_type iuab_lexer_match_token(struct iuab_lexer *lexer) {
    switch (lexer->ch) {
    case 'i': return iuab_lexer_match_end(lexer, "",     IUAB_TOKEN_I);
    case 'u': return iuab_lexer_match_end(lexer, "se",   IUAB_TOKEN_USE);
    case 'a': return iuab_lexer_match_end(lexer, "rch",  IUAB_TOKEN_ARCH);
    case 'l': return iuab_lexer_match_end(lexer, "inux", IUAB_TOKEN_LINUX);
    case 'b':
        switch (iuab_lexer_next_char(lexer)) {
        case 't': return iuab_lexer_match_end(lexer, "w", IUAB_TOKEN_BTW);
        case 'y': return iuab_lexer_match_end(lexer, "",  IUAB_TOKEN_BY);
        }
        break;
    case 't': return iuab_lexer_match_end(lexer, "he",    IUAB_TOKEN_THE);
    case 'w': return iuab_lexer_match_end(lexer, "ay",    IUAB_TOKEN_WAY);
    case 'g': return iuab_lexer_match_end(lexer, "entoo", IUAB_TOKEN_GENTOO);
    }

    return IUAB_TOKEN_INVAL;
}

static struct iuab_token make_token(enum iuab_token_type type, size_t line,
                                    size_t col) {
    return (struct iuab_token) { .type = type, .line = line, .col = col };
}

struct iuab_token iuab_lexer_next_token(struct iuab_lexer *lexer) {
    while (lexer->ch != EOF) {
        if (lexer->ch == '\n') {
            iuab_lexer_new_line(lexer);
        } else if (isspace(lexer->ch)) {
            iuab_lexer_next_char(lexer);
        } else if (lexer->ch == COMMENT_CHAR) {
            iuab_lexer_skip_comment(lexer);
        } else {
            size_t tok_col = lexer->col;
            enum iuab_token_type tok_type = iuab_lexer_match_token(lexer);
            return make_token(tok_type, lexer->line, tok_col);;
        }
    }

    return make_token(IUAB_TOKEN_EOF, lexer->line, lexer->col);
}
