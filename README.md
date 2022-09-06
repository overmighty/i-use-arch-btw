# I use Arch btw

> "I use Arch btw" but it's a Turing-complete programming language.

## Introduction

I use Arch btw is an esoteric programming language based on [Brainfuck](
https://en.wikipedia.org/wiki/Brainfuck) in which the commands are the following
keywords:

`i`, `use`, `arch`, `linux`, `btw`, `by`, `the`, `way`, `gentoo`.

See the [language specification](./docs/language_specification.md) for more
information.

This repository contains a [C/C++ library implementing I use Arch btw](./lib)
and a dependent [command-line interpreter](./cmd).

## Getting Started

### Prerequisites

- [CMake](https://cmake.org/) >= 3.23
- a C99 and C++17 compiler toolchain supported by CMake and providing POSIX
  [`unistd.h`](https://en.wikipedia.org/wiki/Unistd.h)

### Building

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
    $ cmake --build .

### Installation

    # cmake --install .

### Usage

#### Command-line interpreter

    $ i-use-arch-btw <source file>

Try some of the [example I use Arch btw programs](./examples) as source files.

For details:

    $ i-use-arch-btw -h

#### C/C++ library

For documentation of the public API, see the [public headers](
./lib/include/iuab).

For example usage, see the [command-line interpreter](./cmd) and [example
libiuab programs](./examples/libiuab).

## License

This software is licensed under the [GNU General Public License, version 3](
./LICENSE.md).
