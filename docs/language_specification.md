# I use Arch btw language specification

## Source code encoding

I use Arch btw source code is text encoded in ASCII.

## Program resources

I use Arch btw programs are given the following resources:

- a working memory of 65,536 (2^16) 8-bit bytes,
- a pointer initialized to point to the first byte of working memory,
  referred to as the *data pointer*,
- an input file,
- an output file.

They can use these resources through commands represented in source code by
keywords.

## Comments

Source code can contain comments that will be ignored during lexical
analysis. Comments start with the semicolon character (`';'`) and stop at the
end of the line.

## Keywords

Programs are a sequence of commands represented in source code by the following
keywords:

| Keyword  | Meaning                                                                                                                           |
|----------|-----------------------------------------------------------------------------------------------------------------------------------|
| `i`      | Increment the data pointer by 1. It is an execution error to do so when it points to the last byte of the working memory.         |
| `use`    | Decrement the data pointer by 1. It is an execution error to do so when it points to the first byte of the working memory.        |
| `arch`   | Increment the value pointed to by the data pointer by 1. If it was 255, then it becomes 0.                                        |
| `linux`  | Decrement the value pointed to by the data pointer by 1. If it was 0, then it becomes 255.                                        |
| `btw`    | Write the value pointed to by the data pointer as character to the output file.                                                   |
| `by`     | Read a character from the input file into the value pointed to by the data pointer.                                               |
| `the`    | If the value pointed to by the data pointer is zero, then continue execution of the program after the matching `way` keyword.     |
| `way`    | If the value pointed to by the data pointer is not zero, then continue execution of the program after the matching `the` keyword. |
| `gentoo` | Call the debugging event handler.                                                                                                 |

## Whitespace

Keywords are separated by whitespace, defined as a sequence of one or more
of the following characters:

- horizontal tab (`'\t'`),
- new line (`'\n'`),
- carriage return (`'\r'`),
- space (`' '`).
