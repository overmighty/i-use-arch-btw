#include <stdio.h>
#include <string.h>
#include "opcode.h"
#include "util.h"
#include "vm.h"

struct vm *vm_init(const enum opcode *program) {
    struct vm *vm = try_malloc(sizeof(struct vm));

    vm->program = program;
    vm->program_counter = 0;
    memset(vm->memory, 0, VM_MEMORY_CAPACITY * sizeof(uint8_t));
    vm->pointer = 0;
    vm->stack_pointer = 0;

    return vm;
}

static void vm_crash(const struct vm *vm, const char *reason) {
    fprintf(stderr, "error: %s (program counter = %lu)\n",
        reason, vm->program_counter);
    exit(EXIT_FAILURE);
}

static void vm_increment_pointer(struct vm *vm) {
    if (vm->pointer == VM_MEMORY_CAPACITY - 1) {
        vm_crash(vm, "pointer overflow");
    } else {
        vm->pointer++;
    }
}

static void vm_decrement_pointer(struct vm *vm) {
    if (vm->pointer == 0) {
        vm_crash(vm, "pointer underflow");
    } else {
        vm->pointer--;
    }
}

static inline enum opcode vm_peek(const struct vm *vm) {
    return vm->program[vm->program_counter + 1];
}

static void vm_skip_loop(struct vm *vm) {
    enum opcode opcode = vm->program[vm->program_counter];

    while (opcode != OPCODE_LOOP_END && vm_peek(vm) != OPCODE_PROGRAM_END) {
        opcode = vm->program[++vm->program_counter];
    }
}

static void vm_handle_loop_begin(struct vm *vm) {
    if (vm->memory[vm->pointer] == 0) {
        vm_skip_loop(vm);
    } else {
        vm->stack[vm->stack_pointer++] = vm->program_counter;
    }
}

static void vm_handle_loop_end(struct vm *vm) {
    if (vm->memory[vm->pointer] == 0) {
        vm->stack_pointer--;
    } else {
        vm->program_counter = vm->stack[vm->stack_pointer - 1];
    }
}

static void vm_print_debug_info(const struct vm *vm) {
    printf("debug: pointer: %5d, value at pointer: %3d, program counter: %lu\n",
        vm->pointer, vm->memory[vm->pointer], vm->program_counter);
}

void vm_run(struct vm *vm) {
    enum opcode opcode;

    while ((opcode = vm->program[vm->program_counter]) != OPCODE_PROGRAM_END) {
        switch (opcode) {
        case OPCODE_POINTER_INC:
            vm_increment_pointer(vm);
            break;
        case OPCODE_POINTER_DEC:
            vm_decrement_pointer(vm);
            break;
        case OPCODE_VALUE_INC:
            vm->memory[vm->pointer]++;
            break;
        case OPCODE_VALUE_DEC:
            vm->memory[vm->pointer]--;
            break;
        case OPCODE_VALUE_PRINT:
            putchar(vm->memory[vm->pointer]);
            break;
        case OPCODE_VALUE_READ:
            vm->memory[vm->pointer] = fgetc(stdin);
            break;
        case OPCODE_LOOP_BEGIN:
            vm_handle_loop_begin(vm);
            break;
        case OPCODE_LOOP_END:
            vm_handle_loop_end(vm);
            break;
        case OPCODE_DEBUG:
            vm_print_debug_info(vm);
            break;
        }
        vm->program_counter++;
    }
}
