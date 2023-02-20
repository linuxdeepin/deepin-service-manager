cmake_minimum_required(VERSION 3.13)

set(BIN_NAME "qdbus-service-demo")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(${BIN_NAME})

include(GNUInstallDirs)
find_package(Qt5 COMPONENTS Core DBus REQUIRED)
find_package(deepin-qdbus-service REQUIRED)

qt5_add_dbus_adaptor(ADAPTER_SOURCES
                     org.deepin.service.sdk.demo.xml
                     service.h
                     Service)

file(GLOB_RECURSE SRCS
  "main.cpp"
  "service.cpp"
)

add_executable(${BIN_NAME}
    ${SRCS}
    ${ADAPTER_SOURCES}
)

# add_definitions(-DSERVICE_CONFIG_DIR="${CMAKE_INSTALL_PREFIX}/share/deepin-service-manager/")

target_include_directories(${BIN_NAME} PUBLIC
  Qt5::Core
  Qt5::DBus
  ${deepin-qdbus-service_INCLUDE_DIR}
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  ${deepin-qdbus-service_LIBRARIES}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/other/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/sdk-demo.json ${CMAKE_BINARY_DIR}/share/other/)
else()
    install(TARGETS ${BIN_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES sdk-demo.json DESTINATION share/deepin-service-manager/other/)
endif()

install(FILES org.deepin.service.sdk.demo.service DESTINATION share/dbus-1/services/)
