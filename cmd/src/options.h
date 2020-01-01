#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

/* Command-line options. */
struct options {
    bool help; /* Print help mesasge then exit. Default: false. */
    int verbosity; /* Verbosity level. Default: 0. */
    bool version; /* Print version info then exit. Default: false. */
};

/* Initializes the given options struct by setting the default values and then
 * parsing the given command-line options, overriding default option values when
 * needed. Returns if the parsing was successful (true) or not (false). */
bool options_init(struct options *opts, int argc, char **argv);

#endif /* OPTIONS_H */
