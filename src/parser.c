#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "util.h"
#include "vm.h"

#define KEYWORD_COUNT 9

const char *KEYWORDS[] = {
    "i",     // OPCODE_POINTER_INC
    "use",   // OPCODE_POINTER_DEC
    "arch",  // OPCODE_VALUE_INC
    "linux", // OPCODE_VALUE_DEC
    "btw",   // OPCODE_VALUE_PRINT
    "by",    // OPCODE_VALUE_READ
    "the",   // OPCODE_LOOP_BEGIN
    "way",   // OPCODE_LOOP_END
    "gentoo" // OPCODE_DEBUG
};

#define COMMENT_PREFIX ';'

struct parser *parser_init(const char *buffer) {
    struct parser *parser = try_malloc(sizeof(struct parser));

    parser->program = try_malloc(strlen(buffer) * sizeof(enum opcode));
    parser->program_length = 0;
    parser->buffer = buffer;
    parser->cursor = 0;
    parser->line = 1;
    parser->column = 0;
    parser->token = "\0";
    parser->loop_depth = 0;

    return parser;
}

static void crash_parser(const struct parser *parser, const char *format, ...) {
    fprintf(stderr, "error: ");

    // Print crash reason/message
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);

    fprintf(stderr, " (at line %lu, column %lu)\n",
        parser->line, parser->column - strlen(parser->token) + 1);

    exit(EXIT_FAILURE);
}

static inline void parser_handle_new_line(struct parser *parser) {
    parser->line++;
    parser->column = 0;
}

static void parser_skip_comment(struct parser *parser) {
    char c;

    while ((c = parser->buffer[parser->cursor + 1]) != '\0' && c != '\n') {
        parser->column++;
        parser->cursor++;
    }
}

static void parser_get_token(struct parser *parser) {
    size_t token_start = parser->cursor;
    size_t token_length = 1;

    while (isalpha(parser->buffer[parser->cursor + 1])) {
        token_length++;
        parser->column++;
        parser->cursor++;
    }

    parser->token = try_malloc((token_length + 1) * sizeof(char));
    memcpy(parser->token, &parser->buffer[token_start], token_length);
    parser->token[token_length] = '\0';
}

static void parser_loop_depth_check(struct parser *parser, enum opcode opcode) {
    if (opcode == OPCODE_LOOP_BEGIN) {
        if (parser->loop_depth == VM_STACK_SIZE - 1) {
            crash_parser(parser, "too many nested loops");
        } else {
            parser->loop_depth++;
        }
    } else if (opcode == OPCODE_LOOP_END) {
        if (parser->loop_depth == 0) {
            crash_parser(parser, "unexpected loop end");
        } else {
            parser->loop_depth--;
        }
    }
}

static void parser_handle_token(struct parser *parser) {
    bool invalid_token = true;

    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(parser->token, KEYWORDS[i]) == 0) {
            parser->program[parser->program_length++] = (enum opcode) i;
            invalid_token = false;
            parser_loop_depth_check(parser, (enum opcode) i);
            break;
        }
    }

    if (invalid_token) {
        crash_parser(parser, "invalid token '%s'", parser->token);
    }
}

static void parser_finalize_program(struct parser *parser) {
    parser->program[parser->program_length++] = OPCODE_PROGRAM_END;
    size_t size = parser->program_length * sizeof(enum opcode);
    parser->program = try_realloc(parser->program, size);
}

enum opcode *parser_run(struct parser *parser) {
    char c;

    while ((c = parser->buffer[parser->cursor]) != '\0') {
        parser->column++;

        if (c == '\n') {
            parser_handle_new_line(parser);
            parser->cursor++;
            continue;
        }

        if (isspace(c)) {
            parser->cursor++;
            continue;
        }

        if (c == COMMENT_PREFIX) {
            parser_skip_comment(parser);
            parser->cursor++;
            continue;
        }

        parser_get_token(parser);
        parser_handle_token(parser);
        free(parser->token);

        parser->cursor++;
    }

    parser_finalize_program(parser);
    return parser->program;
}
