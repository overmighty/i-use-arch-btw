# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

add_executable(token-printer main.c)

target_compile_features(token-printer PUBLIC c_std_99)
target_compile_options(
    token-printer PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall -Wextra -pedantic>
)

target_link_libraries(token-printer PRIVATE iuab)
