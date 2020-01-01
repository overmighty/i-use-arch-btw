#include "log.h"

#include <stdarg.h>
#include <stdio.h> /* for printf, vprintf */

#define PREFIX_INFO  "\033[1;32minfo: \033[0m"
#define PREFIX_ERROR "\033[1;31merror: \033[0m"

enum log_level log_level = LOG_LEVEL_ERROR;

void log_set_level(enum log_level level) {
    log_level = level;
}

void log_info(const char *fmt, ...) {
    if (log_level <= LOG_LEVEL_INFO) {
        va_list args;
        va_start(args, fmt);
        printf(PREFIX_INFO);
        vprintf(fmt, args);
        putchar('\n');
        va_end(args);
    }
}

void log_error(const char *fmt, ...) {
    if (log_level <= LOG_LEVEL_ERROR) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, PREFIX_ERROR);
        vfprintf(stderr, fmt, args);
        putc('\n', stderr);
        va_end(args);
    }
}
