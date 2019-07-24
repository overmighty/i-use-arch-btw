#ifndef SAFETY_H
#define SAFETY_H

#include "vm.h"

void safety_check_pointer_overflow(const struct vm *vm);

void safety_check_pointer_underflow(const struct vm *vm);

void safety_check_stack_pointer_overflow(const struct vm *vm);

void safety_check_stack_pointer_underflow(const struct vm *vm);

#endif
