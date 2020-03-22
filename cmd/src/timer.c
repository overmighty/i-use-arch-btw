#include "timer.h"

#include "log.h"

#include <time.h>

/* 1 second converted to nanoseconds. */
#define S_TO_NS (1000 * 1000 * 1000)

static struct timespec start;

static double seconds(const struct timespec *t) {
    return t->tv_sec + ((double) t->tv_nsec / S_TO_NS);
}

void timer_start(void) {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void timer_stop(const char *msg) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    double diff = seconds(&end) - seconds(&start);
    log_info("%s %.3fs", msg, diff);
}
