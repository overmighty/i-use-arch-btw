#include "timer.h"

#include "log.h"

#include <sys/timeb.h>

static struct timeb tm_start;

void timer_start(void) {
    ftime(&tm_start);
}

void timer_stop(const char *msg) {
    struct timeb tm_end;
    ftime(&tm_end);
    double diff = tm_end.time - tm_start.time +
        ((double) (tm_end.millitm - tm_start.millitm) / 1000);
    log_info("%s %.3fs", msg, diff);
}
