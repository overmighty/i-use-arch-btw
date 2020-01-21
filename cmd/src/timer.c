#include "timer.h"

#include "log.h"

#include <sys/timeb.h>

/* 1 second converted to milliseconds. */
#define S_TO_MS 1000

static struct timeb tm_start;

void timer_start(void) {
    ftime(&tm_start);
}

void timer_stop(const char *msg) {
    struct timeb tm_end;
    ftime(&tm_end);
    double diff = (double) tm_end.time - tm_start.time +
        ((double) (tm_end.millitm - tm_start.millitm) / S_TO_MS);
    log_info("%s %.3fs", msg, diff);
}
