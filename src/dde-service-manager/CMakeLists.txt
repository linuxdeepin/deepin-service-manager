set(BIN_NAME "dde-service-manager")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(${BIN_NAME})

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd) # sdbus

# Sources files
file(GLOB_RECURSE SRCS 
  "*.h"
  "*.cpp"
  "${SRC_BASE_DIR}/service/*.cpp"
  "${SRC_BASE_DIR}/service/*.h"
  "${SRC_BASE_DIR}/policy/*.cpp"
  "${SRC_BASE_DIR}/policy/*.h"
  "${SRC_BASE_DIR}/plugin-config/*.cpp"
  "${SRC_BASE_DIR}/plugin-config/*.h"
  "${SRC_BASE_DIR}/libddeqdbusservice/*.cpp"
  "${SRC_BASE_DIR}/libddeqdbusservice/*.h"
)

add_executable(${BIN_NAME}  ${SRCS})

target_include_directories(${BIN_NAME} PUBLIC
  Qt5::Core
  Qt5::DBus
  PkgConfig::Systemd # sdbus
  ${SRC_BASE_DIR}
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  PkgConfig::Systemd # sdbus
)

install(TARGETS ${BIN_NAME} DESTINATION bin)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib/other/dde-qt-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/manager.json ${CMAKE_BINARY_DIR}/lib/other/dde-qt-service/)
else()
    install(TARGETS ${BIN_NAME} DESTINATION bin)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manager.json DESTINATION share/dde-service-manager/other/dde-qt-service/)
endif()

# TODO install