
set(BIN_NAME "dde-service-manager-pluginqt-demo1")

set(Plugin_NAME "dde-service-manager-pluginsdbus-demo1")

find_package(PkgConfig REQUIRED)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd)

# Sources files
file(GLOB_RECURSE SRCS "*.h" "*.cpp")

add_library(${Plugin_NAME} MODULE
    ${SRCS}
)

target_include_directories(${Plugin_NAME} PUBLIC
    PkgConfig::Systemd
)

target_link_libraries(${Plugin_NAME}
    PkgConfig::Systemd
    -lpthread
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/user/plugin-sd-service/)
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib/user/plugin-sd-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/plugin-sdbus-demo1.json ${CMAKE_BINARY_DIR}/lib/user/plugin-sd-service/)
else()
    # install(TARGETS ${BIN_NAME} DESTINATION share/dde-service-manager/user/plugin-sd-service/)
    # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-sdbus-demo1.json DESTINATION share/dde-service-manager/user/plugin-sd-service/)
endif()