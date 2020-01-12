<div align="center">
    <h1>I use Arch btw</h1>
    <blockquote>
        <p>"I use Arch btw" but it's a Turing-complete programming language.</p>
    </blockquote>
    <a href="./examples">Example programs</a>
    <span>|</span>
    <a href="./docs/LANG_SPEC.md">Language specification</a>
    <span>|</span>
    <a href="./docs/VM_SPEC.md">Virtual machine specification</a>
</div>

## Introduction

"I use Arch btw" is an esoteric programming language based on
[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) in which the Brainfuck
commands have been replaced with the following keywords:

`i`, `use`, `arch`, `linux`, `btw`, `by`, `the`, `way`.

This repository contains the source code for an "I use Arch btw"
[C/C++ library](./lib) and [command-line interpreter](./cmd), both written in
C99.

## Installation

On Unix-like systems, the Makefiles included in this repository can be used to
build and install both the library and the interpreter:

**Note:** By default, built libraries are installed to `/usr/local/lib`, library
headers are installed to `/usr/local/include`, and built executables are
installed to `/usr/local/bin`.

```
$ cd lib
$ make
$ sudo make install
$ cd ../cmd
$ make
$ sudo make install
```

The common `DESTDIR` and `PREFIX` make flags are supported.

To uninstall the library and the interpreter:

```
$ cd lib
$ sudo make uninstall
$ cd ../cmd
$ sudo make uninstall
```

## Usage

To interpret a file containing "I use Arch btw" source code:

```
$ i-use-arch-btw path/to/file.archbtw
```

Type this command to get the command-line interpreter's help message:

```
$ i-use-arch-btw -h
```

## Building

#### Dependencies

* [`libiuab`](./lib): C/C++ library
    - standard C99 library
* [`i-use-arch-btw`](./cmd): command-line interpreter
    - standard C99 library
    - [`libiuab`](./lib)
    - `unistd.h` (for `getopt`)
    - `sys/timeb.h` (for printing compilation and execution times in verbose
    mode)

#### Makefiles

Both the [`lib`](./lib) directory and the [`cmd`](./cmd) directory contain a
Makefile that allows to easily build the C/C++ library (libiuab) and the
command-line interpreter respectively:

```
$ make
```

By default, when building the library, both the static library and the shared
library will be output to `lib/build`, and when building the command-line
interpreter, the executable will be output to the `cmd/build` directory.

To delete build files/artifacts:

```
$ make clean
```

## License

This project is licensed under the [MIT license](./LICENSE).
