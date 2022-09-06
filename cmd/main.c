// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "compile_and_run.h"
#include "version.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_help(FILE *file, const char *argv0) {
    fprintf(
        file,
        "Usage: %s [options] <source file>\n"
        "\n"
        "Options:\n"
        "  -h  Display this help information then exit.\n"
        "  -V  Display version information then exit.\n",
        argv0
    );
}

void print_version(void) {
    puts(VERSION_STRING);
}

struct options {
    bool help;
    bool version;
};

int options_init(struct options *opts, int argc, char *argv[]) {
    opts->help = false;
    opts->version = false;

    int opt;

    while ((opt = getopt(argc, argv, "h?V")) != -1) {
        switch (opt) {
        case 'h': opts->help = true; break;
        case 'V': opts->version = true; break;
        default: return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    struct options opts;
    int status = options_init(&opts, argc, argv);

    if (status != EXIT_SUCCESS) {
        return status;
    }

    if (opts.help) {
        print_help(stdout, argv[0]);
        return EXIT_SUCCESS;
    }

    if (opts.version) {
        print_version();
        return EXIT_SUCCESS;
    }

    return compile_and_run(argv[1]);
}
