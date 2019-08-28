<div align="center">
    <h1>I use Arch btw</h1>
    <blockquote>
        <p>"I use Arch btw" but it's a Turing-complete programming language.</p>
    </blockquote>
    <a href="./SPECIFICATION.md">Language specification</a>
    <span>|</span>
    <a href="./DOCUMENTATION.md">Interpreter documentation</a>
    <span>|</span>
    <a href="./examples">Examples</a>
</div>

## Introduction

"I use Arch btw" is an esoteric programming language based on
[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) in which the Brainfuck
commands have been replaced with the following keywords:

`i`, `use`, `arch`, `linux`, `btw`, `by`, `the`, `way`.

This repository contains the source code for an "I use Arch btw" interpreter
written in C99.

## Installation

On Unix-like systems, the [Makefile](./Makefile) included in this repository can
be used to build and install the interpreter:

```
$ make
$ sudo make install
```

By default, the Makefile uses `cc` as a C compiler, names the executable
`i-use-arch-btw` and the `install` target copies the executable to
`/usr/local/bin`.

To uninstall the interpreter:

```
$ sudo make uninstall
```

## Usage

```
$ i-use-arch-btw <file>
```

## Building

To build the interpreter with optimizations and without debugging symbols:

```
$ make
```

To build the interpreter without optimizations and with debugging symbols:

```
$ make DEBUG=1
```

The executable will be located in the `build` directory.

To delete build files/artifacts:

```
$ make clean
```

---

This project is licensed under the MIT license.
