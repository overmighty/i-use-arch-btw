#ifndef IUAB_ERRORS_H
#define IUAB_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Library error codes. */
enum iuab_error {
    /* General error codes. */
    IUAB_ERROR_SUCCESS, /* Success. */
    IUAB_ERROR_MEMORY,  /* Memory allocation failure. */
    /* Compiler error codes. */
    IUAB_ERROR_INVALTOK, /* Invalid token. */
    IUAB_ERROR_DEPTHMIN, /* Unexpected loop end. */
    IUAB_ERROR_DEPTHMAX, /* Maximum loop nesting depth exceeded. */
    IUAB_ERROR_DEPTHNZ,  /* Unclosed loops. */
    /* VM error codes. */
    IUAB_ERROR_INVALOP, /* Invalid opcode. */
    IUAB_ERROR_POVERF,  /* Data pointer overflow. */
    IUAB_ERROR_PUNDERF, /* Data pointer underflow. */
    IUAB_ERROR_INEOF    /* End of input file. */
};

/* Returns a string describing the meaning of the given library error code. */
char *iuab_strerror(enum iuab_error error);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_ERRORS_H */
