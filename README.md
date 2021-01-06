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
$ make
$ sudo make install
```

The common `DESTDIR` and `PREFIX` Make flags are supported.

To uninstall the library and the interpreter:

```
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
    - C99 standard library
* [`i-use-arch-btw`](./cmd): command-line interpreter
    - C99 standard library
    - [`libiuab`](./lib)
    - `unistd.h` for `getopt()` and `optind`
    - `time.h` for `clock_gettime()`, `CLOCK_MONOTONIC` and `struct timespec`

#### Makefiles

You can use the global Makefile at the root of this repository to build
everything:

```
$ make
```

To build everything but for debugging (generate debugging symbols):

```
$ make DEBUG=1
```

All build files/artifacts will be located in the build directory specified by
the `BUILDDIR` Make variable, which is set to `build` by default.

To delete build files/artifacts:

```
$ make clean
```

## License

This project is licensed under the [MIT license](./LICENSE).
