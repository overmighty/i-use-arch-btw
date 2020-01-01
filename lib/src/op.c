#include <iuab/op.h>

char *iuab_op_name(uint8_t op) {
    switch (op) {
    case IUAB_OP_RET:   return "RET";
    case IUAB_OP_INCP:  return "INCP";
    case IUAB_OP_DECP:  return "DECP";
    case IUAB_OP_INCV:  return "INCV";
    case IUAB_OP_DECV:  return "DECV";
    case IUAB_OP_READ:  return "READ";
    case IUAB_OP_WRITE: return "WRITE";
    case IUAB_OP_JMPZ:  return "JMPZ";
    case IUAB_OP_JMPNZ: return "JMPNZ";
    case IUAB_OP_DEBUG: return "DEBUG";
    default:            return "???";
    }
}
