
set(Plugin_NAME "plugin-sdbus-demo1")

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
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/user/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/plugin-sdbus-demo1.json ${CMAKE_BINARY_DIR}/share/user/)
else()
    # install(TARGETS ${Plugin_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-service-manager/)
    # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-sdbus-demo1.json DESTINATION share/deepin-service-manager/user/)
endif()
