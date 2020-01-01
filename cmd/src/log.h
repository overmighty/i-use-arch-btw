#ifndef LOG_H
#define LOG_H

/* Log priority levels. */
enum log_level {
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR
};

/* Sets the log level. Logs of a lesser priority level will not be printed. */
void log_set_level(enum log_level level);

/* Logs an info message. Takes the same parameters as printf() but appends a
 * newline to the output. */
void log_info(const char *fmt, ...);

/* Logs an error message. Takes the same parameters as printf() but appends a
 * newline to the output. */
void log_error(const char *fmt, ...);

#endif /* LOG_H */
