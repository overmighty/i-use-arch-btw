## "I use Arch btw" language specification

### Introduction

"I use Arch btw" is an esoteric programming language based on
[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck).

### Keywords

The 8 Brainfuck commands have been replaced with 8 lowercase keywords that must
be separated by whitespace:

`i`, `use`, `arch`, `linux`, `btw`, `by`, `the`, `way`.

A ninth keyword, `gentoo`, can be used to print debugging information.

Here is a comparison of "I use Arch btw" keywords with Brainfuck commands:

| I use Arch btw | Brainfuck | Description                                     |
| -------------- | --------- | ----------------------------------------------- |
| `i`            | `>`       | Increment pointer.                              |
| `use`          | `<`       | Decrement pointer.                              |
| `arch`         | `+`       | Increment value at pointer.                     |
| `linux`        | `-`       | Decrement value at pointer.                     |
| `btw`          | `.`       | Print value at pointer as character.            |
| `by`           | `,`       | Read one char from stdin into value at pointer. |
| `the`          | `[`       | Begin loop.                                     |
| `way`          | `]`       | End loop.                                       |
| `gentoo`       | N/A       | Print debugging information.                    |

### Comments

While in Brainfuck, all characters that aren't a command are ignored, in
"I use Arch btw," only comments are ignored. Comments are single-line and must
start with a semicolon (`;`).

Multi-line comments are not supported.
