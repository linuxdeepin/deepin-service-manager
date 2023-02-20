set(BIN_NAME "deepin-service-manager")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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
  ${SRC_BASE_DIR}/core
  ${SRC_BASE_DIR}/libqdbusservice
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  core-service
)

install(TARGETS ${BIN_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

