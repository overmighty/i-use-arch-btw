# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

add_library(
    iuab
    src/buffer.c
    src/context.c
    src/errors.c
    src/lexer.c
    src/targets/bytecode.c
    src/targets/bytecode_compile.c
    src/targets/bytecode_run.c
    src/targets.c
    src/targets/jit_x86_64_compile.c
    src/targets/jit_x86_64_run.c
    src/token.c
)

target_compile_features(iuab PUBLIC c_std_99)
target_compile_options(
    iuab PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall -Wextra -pedantic>
)

configure_file(include/iuab/version.h.in include/iuab/version.h)
target_sources(
    iuab PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}/include
    FILES
    include/iuab/buffer.h
    include/iuab/context.h
    include/iuab/errors.h
    include/iuab/lexer.h
    include/iuab/targets/bytecode.h
    include/iuab/targets.h
    include/iuab/targets/jit_x86_64.h
    include/iuab/token.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/iuab/version.h
)

set_target_properties(
    iuab
    PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
)

install(
    TARGETS iuab
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)
