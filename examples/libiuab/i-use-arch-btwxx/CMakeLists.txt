# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

add_executable(i-use-arch-btwxx main.cpp)

target_compile_features(i-use-arch-btwxx PUBLIC cxx_std_17)
target_compile_options(
    i-use-arch-btwxx PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Wall -Wextra -pedantic>
)

target_link_libraries(i-use-arch-btwxx PRIVATE iuab)
