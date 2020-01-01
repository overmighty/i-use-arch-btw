#include "options.h"

#include <unistd.h> /* for getopt */

static void options_set_defaults(struct options *opts) {
    opts->verbosity = 0;
    opts->version = false;
}

static bool options_parse(struct options *opts, int argc, char **argv) {
    int opt;

    while ((opt = getopt(argc, argv, "hvV")) != -1) {
        switch (opt) {
        case 'h': opts->help = true; break;
        case 'v': opts->verbosity++; break;
        case 'V': opts->version = true; break;
        default:  return true;
        }
    }

    return false;
}

bool options_init(struct options *opts, int argc, char **argv) {
    options_set_defaults(opts);
    return options_parse(opts, argc, argv);
}
