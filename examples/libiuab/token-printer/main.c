// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/lexer.h"
#include "iuab/token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_tokens(const char *filename) {
    FILE *src = fopen(filename, "rbe");

    if (!src) {
        perror("error: failed to open source file");
        return EXIT_FAILURE;
    }

    struct iuab_lexer lexer;
    iuab_lexer_init(&lexer, src);

    size_t last_line = 1;
    size_t last_end_col = 1;
    struct iuab_token token;

    while ((token = iuab_lexer_next_token(&lexer)).type != IUAB_TOKEN_EOF) {
        if (token.type == IUAB_TOKEN_INVALID) {
            fprintf(
                stderr,
                "error: invalid token at line %zu, col %zu\n",
                token.line,
                token.col
            );
            fclose(src);
            return EXIT_FAILURE;
        }

        while (token.line > last_line) {
            last_end_col = 1;
            putchar('\n');
            last_line++;
        }

        const char *str = iuab_token_type_name(token.type);
        printf("%*s%s", (int) (token.col - last_end_col), "", str);
        last_end_col = token.col + strlen(str);
    }

    fclose(src);
    putchar('\n');
    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return print_tokens(argv[1]);
}
