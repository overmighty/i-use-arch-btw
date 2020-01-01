#ifndef IUAB_OP_H
#define IUAB_OP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> /* for uint8_t */

/* Opcodes for the virtual machine. */
enum {
    IUAB_OP_RET,   /* Return from the execution loop. */
    IUAB_OP_INCP,  /* Increase data pointer. */
    IUAB_OP_DECP,  /* Decrease data pointer. */
    IUAB_OP_INCV,  /* Increase value at data pointer. */
    IUAB_OP_DECV,  /* Decrease value at data pointer. */
    IUAB_OP_READ,  /* Read character into value at data pointer. */
    IUAB_OP_WRITE, /* Write value at data pointer as character. */
    IUAB_OP_JMPZ,  /* Jump if value at data pointer is zero. */
    IUAB_OP_JMPNZ, /* Jump if value at data pointer is not zero. */
    IUAB_OP_DEBUG, /* Print debugging information. */

    IUAB_OP_INVAL  /* Invalid operation. */
};

/* Returns the name of the given opcode as a string. */
char *iuab_op_name(uint8_t op);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_OP_H */
