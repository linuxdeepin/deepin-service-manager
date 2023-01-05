
set(BIN_NAME "dde-service-manager-pluginqt-demo1")

project(${BIN_NAME})

file(GLOB_RECURSE SRCS "*.h" "*.cpp")

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

add_library(${BIN_NAME} MODULE
    ${SRCS}
)

target_include_directories(${BIN_NAME} PUBLIC
  Qt5::Core
  Qt5::DBus
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/user/plugin-qt-service/)
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib/user/plugin-qt-service/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/plugin-qt-demo1.json ${CMAKE_BINARY_DIR}/lib/user/plugin-qt-service/)
else()
    # install(TARGETS ${BIN_NAME} DESTINATION share/dde-service-manager/user/plugin-qt-service/)
    # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-qt-demo1.json DESTINATION share/dde-service-manager/user/plugin-qt-service/)
endif()