#ifndef TIMER_H
#define TIMER_H

/* Starts the static timer. */
void timer_start(void);

/* Stops the static timer and writes an info log with the given message followed
 * by the elapsed time since the timer was started. */
void timer_stop(const char *msg);

#endif /* TIMER_H */
