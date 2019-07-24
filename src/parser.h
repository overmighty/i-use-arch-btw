#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include "opcode.h"

struct parser {
    enum opcode *program;
    size_t      program_length;
    const char  *buffer;
    size_t      cursor;
    size_t      line;
    size_t      column;
};

struct parser *parser_init(const char *buffer);

enum opcode *parser_run(struct parser *parser);

#endif
