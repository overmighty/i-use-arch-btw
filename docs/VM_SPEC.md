## "I use Arch btw" virtual machine specification

### Design

The bytecode virtual machine for "I use Arch btw" has a minimal set of
components:

* 64 KiB of program memory (`uint8_t` array) (all bytes initialized to 0)
* Registers
    - a program counter (`size_t`)
    - a data pointer (`uint8_t *`) (initialized to address of first byte of
    program memory in the host machine's memory)
* I/O files
    - an input file, for the running program to read characters from
    - an output file, for the running program to write characters to

### Instruction set

The virtual machine's instruction set is also minimal. Here is a table of all
instructions supported by the virtual machine:

| Opcode | Name    | Operand   | Description                                           |
|--------|---------|-----------|-------------------------------------------------------|
| 0      | `RET`   | None      | Return from the execution loop.                       |
| 1      | `INCP`  | `uint8_t` | Increase data pointer by operand.                     |
| 2      | `DECP`  | `uint8_t` | Decrease data pointer by operand.                     |
| 3      | `INCV`  | `uint8_t` | Increase value at data pointer by operand.            |
| 4      | `DECV`  | `uint8_t` | Decrease value at data pointer by operand.            |
| 5      | `READ`  | None      | Read character into value at data pointer.            |
| 6      | `WRITE` | None      | Write value at data pointer as character.             |
| 7      | `JMPZ`  | `size_t`  | Jump to operand if value at data pointer is zero.     |
| 8      | `JMPNZ` | `size_t`  | Jump to operand if value at data pointer is not zero. |
| 9      | `DEBUG` | None      | Print debugging information.                          |

### Execution loop

Each iteration of the virtual machine's execution loop can be dissected into
these steps:

1.  Read opcode at program counter
2.  Increment program counter
3.  If opcode is `RET`: break loop
    Else: Call handler function for opcode
    1.  Read operand if needed
    2.  If operation is impossible: set error code
        Else: execute operation

### Bytecode encoding

Bytecode programs to be executed by the virtual machine must be encoded in the
following format:

```
<opcode: uint8_t> [operand: uint8_t|size_t] ...
```

Operands of the `size_t` type, the only multi-byte data type in "I use Arch btw"
bytecode, are written in the host machine's endianness.

Here is an example represented in hexadecimal, on a little endian machine on
which `size_t` is an 8-byte data type:

```
03 01 07 16 00 00 00 00 00 00 00 05 06 08 0B 00 00 00 00 00 00 00 00
```

Here is a disassembly of the example bytecode:

| Address | Opcode      | Operand |
|---------|-------------|---------|
| 0x0     | 3 (`INCV`)  | 0x1     |
| 0x2     | 7 (`JMPZ`)  | 0x16    |
| 0xB     | 5 (`READ`)  |         |
| 0xC     | 6 (`WRITE`) |         |
| 0xD     | 8 (`JMPNZ`) | 0xB     |
| 0x16    | 0 (`RET`)   |         |
