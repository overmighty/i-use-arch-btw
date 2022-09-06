# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.23)

project(i-use-arch-btw VERSION 0.1.0)

option(IUAB_BUILD_EXAMPLES "Build the libiuab example programs by default.")
option(
    IUAB_USE_JIT
    "Use JIT compilation in the interpreter if available for the target system."
    ON
)

# Windows does not follow the System V ABI's calling convention, and Apple
# platforms require passing the `MAP_JIT` flag to `mmap()` and the program to
# have a relevant entitlement.
if(IUAB_USE_JIT AND (WIN32 OR APPLE))
    set(IUAB_USE_JIT OFF)
endif()

include(GNUInstallDirs)

add_subdirectory(lib)
add_subdirectory(cmd)

if(IUAB_BUILD_EXAMPLES)
    add_subdirectory(examples/libiuab)
else()
    add_subdirectory(examples/libiuab EXCLUDE_FROM_ALL)
endif()
