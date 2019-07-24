#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "util.h"
#include "vm.h"

static enum opcode *parse_buffer(const char *buffer) {
    struct parser *parser = parser_init(buffer);
    enum opcode *program = parser_run(parser);
    free(parser);
    return program;
}

static void run_interpreter(FILE *file) {
    char *buffer = buffer_from_file(file);
    fclose(file);

    enum opcode *program = parse_buffer(buffer);
    free(buffer);

    struct vm *vm = vm_init(program);
    vm_run(vm);
    free(program);
    free(vm);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Although the interpreter will not write to the file, the "r+b" mode is
    // still used instead of just "rb", as, with glibc on Linux at least, it
    // will make fopen fail if the file is not a regular file but something
    // else, such as a directory
    FILE *file = fopen(argv[1], "r+b");

    if (file == NULL) {
        perror("error: could not read file");
        return EXIT_FAILURE;
    }

    run_interpreter(file);
    return EXIT_SUCCESS;
}
