#include "log.h"
#include "options.h"
#include "timer.h"
#include "version.h"

#include <iuab/compiler.h>
#include <iuab/version.h>
#include <iuab/vm.h>

#include <unistd.h> /* for optind */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *name, FILE *dest) {
    fprintf(dest, "usage: %s [options] [file] [options]\n\
\n\
options:\n\
  -h    print this help message then exit\n\
  -v    increase the verbosity level\n\
  -V    print version info then exit\n\
", name);
}

void print_version() {
    printf("i-use-arch-btw %s\n", INTERPRETER_VERSION);
    printf("libiuab %s\n", IUAB_VERSION_STRING);
}

void log_compiler_error(struct iuab_compiler *comp) {
    if (comp->error == IUAB_ERROR_DEPTHNZ) {
        log_error("%s", iuab_strerror(comp->error));
    } else {
        log_error("%s at line %zu, col %zu", iuab_strerror(comp->error),
            comp->token.line, comp->token.col);
    }
}

bool compile_program(FILE *src, struct iuab_buffer *dest) {
    struct iuab_compiler comp;
    iuab_compiler_init(&comp, src, dest);

    timer_start();
    iuab_compiler_run(&comp);
    timer_stop("compiled in");

    if (comp.error != IUAB_ERROR_SUCCESS) {
        log_compiler_error(&comp);
    }

    return comp.error != IUAB_ERROR_SUCCESS;
}

void print_bytecode(const struct iuab_buffer *code) {
    size_t i = 0;

    while (i < code->len) {
        uint8_t op = code->data[i];
        printf("[0x%0*zX] %-5s", 2 * (int) sizeof(size_t), i, iuab_op_name(op));
        i++;

        /* Print the operand if there is one */
        if (op >= IUAB_OP_INCP && op <= IUAB_OP_DECV) {
            printf(" 0x%X", code->data[i++]);
        } else if (op == IUAB_OP_JMPZ || op == IUAB_OP_JMPNZ) {
            printf(" 0x%zX", *((size_t *) &code->data[i]));
            i += sizeof(size_t);
        }

        putchar('\n');
    }
}

bool execute_program(const struct iuab_buffer *program) {
    struct iuab_vm vm;
    iuab_vm_init(&vm, program->data, stdin, stdout);
    size_t last_op_pos;

    timer_start();
    enum iuab_error result = iuab_vm_run(&vm, &last_op_pos);
    timer_stop("executed in");

    if (result != IUAB_ERROR_SUCCESS) {
        log_error("%s at 0x%zX", iuab_strerror(result), last_op_pos);
        return true;
    }

    return false;
}

int interpret_file(FILE *file, struct options *opts) {
    log_info("compiling...");
    struct iuab_buffer code;
    enum iuab_error result = iuab_buffer_init(&code);

    if (result != IUAB_ERROR_SUCCESS) {
        log_error("failed to init bytecode buffer: %s", iuab_strerror(result));
        return EXIT_FAILURE;
    }

    if (compile_program(file, &code)) {
        return EXIT_FAILURE;
    }

    if (opts->verbosity > 1) {
        log_info("generated bytecode:");
        print_bytecode(&code);
    }

    log_info("executing...");
    int status = execute_program(&code) ? EXIT_FAILURE : EXIT_SUCCESS;
    iuab_buffer_fini(&code);
    return status;
}

int interpret_argv(int argc, char **argv, struct options *opts) {
    if (optind >= argc) {
        print_usage(argv[0], stderr);
        return EXIT_FAILURE;
    }

    FILE *src = fopen(argv[optind], "rbe");

    if (!src) {
        log_error("could not open file: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    int status = interpret_file(src, opts);
    fclose(src);
    return status;
}

int main(int argc, char **argv) {
    struct options opts;

    if (options_init(&opts, argc, argv) || opts.help) {
        print_usage(argv[0], opts.help ? stdout : stderr);
        return opts.help ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (opts.verbosity > 0) {
        log_set_level(LOG_LEVEL_INFO);
    }

    if (opts.version) {
        print_version();
        return EXIT_SUCCESS;
    }

    return interpret_argv(argc, argv, &opts);
}
