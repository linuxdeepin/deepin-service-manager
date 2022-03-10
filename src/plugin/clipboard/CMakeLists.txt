cmake_minimum_required(VERSION 3.7)

set(PLUGIN_NAME "clipboard")

project(${PLUGIN_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Sources files
file(GLOB_RECURSE SRCS "./*.h" "./*.cpp" "src/*.h" "src/*.cpp")

find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(KF5Wayland REQUIRED)

add_definitions("${QT_DEFINITIONS} -DCLIPBOARD_LIBRARY")
add_library(${PLUGIN_NAME} SHARED ${SRCS} )
set_target_properties(${PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../lib/session)

target_include_directories(${PLUGIN_NAME} PUBLIC
  ${Qt5Widget_INCLUDE_DIRS}
  ${Qt5DBus_INCLUDE_DIRS}
  KF5::WaylandServer
  src
  )

target_link_libraries(${PLUGIN_NAME} PRIVATE
  ${Qt5Widgets_LIBRARIES}
  ${Qt5DBus_LIBRARIES}
  KF5::WaylandClient
)

install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-plugins/session)
