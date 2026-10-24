# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

find_program(CARGO_EXECUTABLE cargo REQUIRED)

set(RUST_TARGET_DIR "${CMAKE_BINARY_DIR}/cargo")
set(RUST_PLUGIN "${RUST_TARGET_DIR}/debug/libplugin_rust_demo1.so")

add_custom_command(
    OUTPUT "${RUST_PLUGIN}"
    COMMAND
        ${CMAKE_COMMAND} -E env
        CARGO_TARGET_DIR=${RUST_TARGET_DIR}
        ${CARGO_EXECUTABLE} build
            --locked
            --manifest-path "${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml"
        "${CMAKE_CURRENT_SOURCE_DIR}/Cargo.lock"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lib.rs"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    VERBATIM
)

add_custom_target(plugin-rust-demo1 ALL DEPENDS "${RUST_PLUGIN}")

add_custom_command(
    TARGET plugin-rust-demo1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/share/user"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RUST_PLUGIN}"
        "${CMAKE_BINARY_DIR}/lib/libplugin_rust_demo1.so"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/plugin-rust-demo1.json"
        "${CMAKE_BINARY_DIR}/share/user/plugin-rust-demo1.json"
    VERBATIM
)
