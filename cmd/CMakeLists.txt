# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: GPL-3.0-only

add_executable(i-use-arch-btw compile_and_run.c main.c debug_handler.c)

target_compile_features(i-use-arch-btw PUBLIC c_std_99)
target_compile_options(
    i-use-arch-btw PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall -Wextra -pedantic>
)

if(IUAB_USE_JIT)
    target_compile_definitions(i-use-arch-btw PUBLIC IUAB_USE_JIT)
endif()

configure_file(version.h.in version.h)
target_include_directories(i-use-arch-btw PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(i-use-arch-btw PRIVATE iuab)

set_target_properties(
    i-use-arch-btw
    PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
)

install(
    TARGETS i-use-arch-btw
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
)
