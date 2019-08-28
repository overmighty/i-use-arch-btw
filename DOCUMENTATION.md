## Interpreter documentation

### Memory

The "I use Arch btw" interpreter allocates 2^16 (65,536) memory cells of 8 bits
each, and initializes each cell's value to zero.

### Loops

The interpreter only allows for a maximum of 255 nested loops. This is due to
the fact that instructions only consist of an opcode:

The interpreter has a stack, and when it encounters the `OPCODE_LOOP_BEGIN`
opcode, it pushes the current value of the program counter to the top of that
stack.

When it encounters the `OPCODE_LOOP_END` opcode, it simply sets the program
counter to the value at the top of the stack, then decrements the stack pointer
to act like the value at the top of the stack had been deleted.

This might not be very smart, but we are talking about an esoteric language
anyway, so let's call this limitation a feature and get away with it.

### Safety checks

Whenever the pointer is incremented or decremented, the interpreter checks if
the operation will cause a pointer overflow or underflow, and will crash if that
is the case.

However, there are no safety checks for overflow and underflow of memory cell
values. Therefore, programs can overflow and underflow memory cell values for
optimization/compression, as the interpreter will not crash.

Before a safety check crashes the interpreter, an error message is printed. The
error message gives a description of the cause of the crash and the value of the
program counter, which corresponds to the index of the instruction opcode that
would have been executed if the safety check had not failed. You can find where
in your program the error occurred by counting up to the program counter's value
starting from zero at the first keyword in your program.
