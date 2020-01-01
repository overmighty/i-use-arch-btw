#ifndef IUAB_VM_H
#define IUAB_VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "errors.h"

#include <stdint.h> /* for uint8_t */
#include <stdio.h>  /* for FILE, size_t */

#define IUAB_VM_MEMORY_SIZE (1 << 16)

/* An "I use Arch btw" bytecode virtual machine */
struct iuab_vm {
    const uint8_t *program; /* The bytecode program to run. */
    uint8_t memory[IUAB_VM_MEMORY_SIZE]; /* The memory for the program. */
    size_t pc; /* The program counter. */
    uint8_t *dp; /* The data pointer. */
    FILE *in; /* The input file for the program. */
    FILE *out; /* The output file for the program. */
    const char *debug_fmt; /* The printf format for debug messages. */
    enum iuab_error error; /* The current error. */
};

/* Initializes the given VM for execution of the given bytecode program Sets
 * the VM's input and output files to the in and out parameters respectively. */
void iuab_vm_init(struct iuab_vm *vm, const uint8_t *program, FILE *in,
                  FILE *out);

/* Runs the given VM, returning its error code. Always sets the value pointed to
 * by last_op_pos to the VM's program counter when reading an opcode. */
enum iuab_error iuab_vm_run(struct iuab_vm *vm, size_t *last_op_pos);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_VM_H */
