

set(BIN_NAME "dde-qdbus-service-sdk-demo")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(${BIN_NAME})

find_package(Qt5 COMPONENTS Core Widgets DBus REQUIRED)

qt5_add_dbus_adaptor(ADAPTER_SOURCES
                     org.deepin.services.sdk.demo.xml
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
  ${SRC_BASE_DIR}/libddeqdbusservice
  Qt5::Core
  Qt5::DBus
  Qt5::Widgets
)

target_link_directories(${BIN_NAME} PRIVATE
  ${CMAKE_BINARY_DIR}/src/libddeqdbusservice/
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  Qt5::Widgets
  -ldde-service-manager-dev
)

add_dependencies(${BIN_NAME} "dde-service-manager-dev")
set_target_properties(${BIN_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}/src/libddeqdbusservice/")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib/other/dde-qt-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/sdk-demo.json ${CMAKE_BINARY_DIR}/lib/other/dde-qt-service/)
else()
    # install(TARGETS ${BIN_NAME} DESTINATION bin)
    # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sdk-demo.json DESTINATION share/dde-service-manager/other/dde-qt-service/)
endif()