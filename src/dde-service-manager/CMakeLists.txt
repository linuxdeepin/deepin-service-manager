cmake_minimum_required(VERSION 3.7)

set(BIN_NAME "dde-service-manager")

project(${BIN_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Sources files
file(GLOB_RECURSE SRCS "*.h" "*.cpp")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)

add_executable(${BIN_NAME}  ${SRCS})

target_include_directories(${BIN_NAME} PUBLIC
  ${Qt5Widget_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  )

target_link_libraries(${BIN_NAME} PRIVATE
  ${Qt5DBus_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
)

install(TARGETS ${BIN_NAME} DESTINATION bin)
