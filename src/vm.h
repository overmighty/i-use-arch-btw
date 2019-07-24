#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdlib.h>
#include "opcode.h"

#define VM_MEMORY_CAPACITY (1 << 16)
#define VM_STACK_SIZE      (1 << 8)

struct vm {
    const enum opcode *program;
    size_t             program_counter;
    uint8_t            memory[VM_MEMORY_CAPACITY];
    uint16_t           pointer;
    size_t             stack[VM_STACK_SIZE];
    uint8_t            stack_pointer;
};

struct vm *vm_init(const enum opcode *program);

void vm_run(struct vm *vm);

#endif
