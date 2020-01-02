#include <iuab/vm.h>

#include <iuab/op.h>

#include <string.h> /* for memset */

#define DEFAULT_DEBUG_FMT "\033[1;34mdebug: \033[0mpc=0x%zX dp=0x%zX *dp=0x%X\n"

void iuab_vm_init(struct iuab_vm *vm, const uint8_t *program, FILE *in,
                  FILE *out) {
    vm->program = program;
    memset(vm->memory, 0, IUAB_VM_MEMORY_SIZE);
    vm->pc = 0;
    vm->dp = vm->memory;
    vm->in = in;
    vm->out = out;
    vm->debug_fmt = DEFAULT_DEBUG_FMT;
    vm->error = IUAB_ERROR_SUCCESS;
}

static uint8_t iuab_vm_read_byte(struct iuab_vm *vm) {
    return vm->program[vm->pc++];
}

static size_t iuab_vm_read_size(struct iuab_vm *vm) {
    size_t size = *((size_t *) &vm->program[vm->pc]);
    vm->pc += sizeof(size_t);
    return size;
}

static void iuab_vm_op_incp(struct iuab_vm *vm) {
    uint8_t value = iuab_vm_read_byte(vm);

    if (vm->dp >= vm->memory + IUAB_VM_MEMORY_SIZE - value) {
        vm->error = IUAB_ERROR_POVERF;
    } else {
        vm->dp += value;
    }
}

static void iuab_vm_op_decp(struct iuab_vm *vm) {
    uint8_t value = iuab_vm_read_byte(vm);

    if (vm->dp < vm->memory + value) {
        vm->error = IUAB_ERROR_PUNDERF;
    } else {
        vm->dp -= value;
    }
}

static void iuab_vm_op_incv(struct iuab_vm *vm) {
    *vm->dp += iuab_vm_read_byte(vm);
}

static void iuab_vm_op_decv(struct iuab_vm *vm) {
    *vm->dp -= iuab_vm_read_byte(vm);
}

static void iuab_vm_op_write(struct iuab_vm *vm) {
    fputc(*vm->dp, vm->out);
}

static void iuab_vm_op_read(struct iuab_vm *vm) {
    int c = fgetc(vm->in);

    if (c == EOF) {
        vm->error = IUAB_ERROR_INEOF;
    } else {
        *vm->dp = c;
    }
}

static void iuab_vm_op_jmpz(struct iuab_vm *vm) {
    size_t value = iuab_vm_read_size(vm);

    if (!*vm->dp) {
        vm->pc = value;
    }
}

static void iuab_vm_op_jmpnz(struct iuab_vm *vm) {
    size_t value = iuab_vm_read_size(vm);

    if (*vm->dp) {
        vm->pc = value;
    }
}

static void iuab_vm_op_debug(struct iuab_vm *vm) {
    fprintf(vm->out, vm->debug_fmt, vm->pc - 1, vm->dp - vm->memory, *vm->dp);
}

enum iuab_error iuab_vm_run(struct iuab_vm *vm, size_t *last_op_pos) {
    uint8_t op;

    while ((op = iuab_vm_read_byte(vm)) != IUAB_OP_RET) {
        *last_op_pos = vm->pc - 1;

        switch (op) {
        case IUAB_OP_INCP:  iuab_vm_op_incp(vm); break;
        case IUAB_OP_DECP:  iuab_vm_op_decp(vm); break;
        case IUAB_OP_INCV:  iuab_vm_op_incv(vm); break;
        case IUAB_OP_DECV:  iuab_vm_op_decv(vm); break;
        case IUAB_OP_READ:  iuab_vm_op_read(vm); break;
        case IUAB_OP_WRITE: iuab_vm_op_write(vm); break;
        case IUAB_OP_JMPZ:  iuab_vm_op_jmpz(vm); break;
        case IUAB_OP_JMPNZ: iuab_vm_op_jmpnz(vm); break;
        case IUAB_OP_DEBUG: iuab_vm_op_debug(vm); break;
        default:            vm->error = IUAB_ERROR_INVALOP;
        }

        if (vm->error != IUAB_ERROR_SUCCESS) {
            return vm->error;
        }
    }

    return vm->error;
}
