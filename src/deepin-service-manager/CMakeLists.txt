set(BIN_NAME "deepin-service-manager")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(${BIN_NAME})

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

# Sources files
file(GLOB_RECURSE SRCS
  "*.h"
  "*.cpp"
  "${SRC_BASE_DIR}/libqdbusservice/*.cpp"
  "${SRC_BASE_DIR}/libqdbusservice/*.h"
)

add_executable(${BIN_NAME}  ${SRCS})

target_compile_definitions(${BIN_NAME} PRIVATE
  VERSION="${VERSION}"
)

target_link_directories(${BIN_NAME} PRIVATE
  ${CMAKE_BINARY_DIR}/core/
)

target_include_directories(${BIN_NAME} PUBLIC
  Qt5::Core
  Qt5::DBus
  PkgConfig::Systemd # sdbus
  ${SRC_BASE_DIR}
  ${SRC_BASE_DIR}/core
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  core-service
)

install(TARGETS ${BIN_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/other/qt-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/manager.json ${CMAKE_BINARY_DIR}/share/other/qt-service/)
else()
    install(TARGETS ${BIN_NAME} DESTINATION bin)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manager.json DESTINATION share/deepin-service-manager/other/qt-service/)
endif()

# TODO install
