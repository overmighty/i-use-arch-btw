## "I use Arch btw" language specification

### Design

"I use Arch btw" is an esoteric programming language based on
[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck).

Similarly to Brainfuck, "I use Arch btw" is designed around 65,536 (2¹⁶)
bytes of memory initialized to 0 and a movable data pointer that is
initialized to point to the first memory byte, as well as an input file and an
output file, which are respectively `stdin` and `stdout` in the command-line
interpreter.

### Keywords

"I use Arch btw" supports the same basic operations as Brainfuck, but is based
on a set of keywords instead of a set of command characters.

Keywords must of course be separated by whitespace. In "I use Arch btw",
whitespace is defined as any character that makes the `isspace` function of the
standard C header `ctype.h` return a truthy (non-zero) value with the default
"C" locale.

Here is a table of all "I use Arch btw" keywords and their Brainfuck command
character equivalents:

| I use Arch btw | Brainfuck | Description                                            |
|----------------|-----------|--------------------------------------------------------|
| `i`            | `>`       | Increment data pointer.                                |
| `use`          | `<`       | Decrement data pointer.                                |
| `arch`         | `+`       | Increment value at data pointer.                       |
| `linux`        | `-`       | Decrement value at data pointer.                       |
| `btw`          | `.`       | Write value at data pointer to output as character.    |
| `by`           | `,`       | Read character from input into value at data pointer.  |
| `the`          | `[`       | Begin loop. (While value at data pointer is not zero.) |
| `way`          | `]`       | End loop.                                              |
| `gentoo`       | N/A       | Print debugging information.                           |

### Comments

Since Brainfuck is based on a set of command characters, it can simply ignore
characters that are not included in the said set.

However, as "I use Arch btw" is based on a set of keywords, it introduces
comments, like most programming languages.

In "I use Arch btw", comments start with a semicolon (`;`) in a line of source
code and continue until the end of that line.

Multi-line comments are not supported.

### Debugging information

The default format for the debugging information that the `gentoo` keyword
causes to print is the following:

```
\033[1;34mdebug: \033[0mpc=0x%zX dp=0x%zX *dp=0x%X\n
```

Debug logs are formatted with the following values in the given order:

* a `size_t`: the previous value of the program counter, thus the index, in the
bytecode buffer, of the opcode that matches the `gentoo` keyword in the source
code that caused the debug log
* a `size_t`: the virtual address currently stored in the data pointer
* a `uint8_t`: the current value of the byte that the data pointer points to

Here is an example debug log with the default format:

```
debug: pc=0x36 dp=0x0 *dp=0xA
```

Here, the first value (the previous value of the program counter) is `0x36`, or
36 in hexadecimal and 54 in decimal.

The second value (the virtual address stored in the data pointer) is `0x0`: 0.

The third value (the current value at the data pointer) is `0xA`, or A in
hexadecimal and 10 in decimal.
