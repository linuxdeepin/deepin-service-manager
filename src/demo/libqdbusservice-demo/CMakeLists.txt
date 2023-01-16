

set(BIN_NAME "qdbus-service-demo")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(${BIN_NAME})

include(GNUInstallDirs)
find_package(Qt5 COMPONENTS Core Widgets DBus REQUIRED)

qt5_add_dbus_adaptor(ADAPTER_SOURCES
                     org.deepin.service.sdk.demo.xml
                     service.h
                     Service)

file(GLOB_RECURSE SRCS
  "*.cpp"
  "*.h"
)

add_executable(${BIN_NAME}
    ${SRCS}
    ${ADAPTER_SOURCES}
)

target_include_directories(${BIN_NAME} PUBLIC
  ${SRC_BASE_DIR}/libqdbusservice
  Qt5::Core
  Qt5::DBus
  Qt5::Widgets
)

target_link_directories(${BIN_NAME} PRIVATE
  ${CMAKE_BINARY_DIR}/src/libqdbusservice/
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  Qt5::Widgets
  qdbus-service
)

add_dependencies(${BIN_NAME} "qdbus-service")
set_target_properties(${BIN_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}/src/libqdbusservice/")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/other/qt-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/sdk-demo.json ${CMAKE_BINARY_DIR}/share/other/qt-service/)
else()
    # install(TARGETS ${BIN_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sdk-demo.json DESTINATION share/deepin-service-manager/other/qt-service/)
endif()
