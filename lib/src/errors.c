#include <iuab/errors.h>

char *iuab_strerror(enum iuab_error error) {
    switch (error) {
    /* General error codes */
    case IUAB_ERROR_SUCCESS: return "success";
    case IUAB_ERROR_MEMORY:  return "memory allocation failure";
    /* Compiler error codes */
    case IUAB_ERROR_INVALTOK: return "invalid token";
    case IUAB_ERROR_DEPTHMIN: return "unexpected loop end";
    case IUAB_ERROR_DEPTHMAX: return "maximum loop nesting depth exceeded";
    case IUAB_ERROR_DEPTHNZ:  return "unclosed loops";
    /* VM error codes */
    case IUAB_ERROR_INVALOP: return "invalid opcode";
    case IUAB_ERROR_POVERF:  return "data pointer overflow";
    case IUAB_ERROR_PUNDERF: return "data pointer underflow";
    case IUAB_ERROR_INEOF:   return "end of input file";
    /* Unknown error code */
    default: return "unknown error";
    }
}
