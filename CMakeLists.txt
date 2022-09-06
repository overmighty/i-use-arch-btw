# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.23)

project(i-use-arch-btw VERSION 0.1.0)

option(IUAB_BUILD_EXAMPLES "Build the libiuab example programs by default.")

include(GNUInstallDirs)

add_subdirectory(lib)
add_subdirectory(cmd)

if(IUAB_BUILD_EXAMPLES)
    add_subdirectory(examples/libiuab)
else()
    add_subdirectory(examples/libiuab EXCLUDE_FROM_ALL)
endif()
