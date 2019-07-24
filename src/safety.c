#include <stdbool.h>
#include <stdio.h>
#include "safety.h"
#include "util.h"

static void crash(const struct vm *vm, const char *reason) {
    fprintf(stderr, "error: %s (program counter = %lu)\n",
        reason, vm->program_counter);
    exit(EXIT_FAILURE);
}

void safety_check_pointer_overflow(const struct vm *vm) {
    if (vm->pointer == VM_MEMORY_CAPACITY - 1) {
        crash(vm, "pointer overflow");
    }
}

void safety_check_pointer_underflow(const struct vm *vm) {
    if (vm->pointer == 0) {
        crash(vm, "pointer underflow");
    }
}

void safety_check_stack_pointer_overflow(const struct vm *vm) {
    if (vm->stack_pointer == VM_STACK_SIZE - 1) {
        crash(vm, "stack pointer overflow:\n\ttoo many nested loops");
    }
}

void safety_check_stack_pointer_underflow(const struct vm *vm) {
    if (vm->stack_pointer == 0) {
        crash(vm, "stack pointer underflow:\n\tunexpected loop end");
    }
}
