

set(LIB_NAME "qdbus-service")

project(${LIB_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd) # sdbus

file(GLOB_RECURSE LIB_SRCS
  "*.h"
  "*.cpp"
  "${SRC_BASE_DIR}/service/servicebase.cpp"
  "${SRC_BASE_DIR}/service/servicebase.h"
  "${SRC_BASE_DIR}/service/serviceqtdbus.cpp"
  "${SRC_BASE_DIR}/service/serviceqtdbus.h"
  "${SRC_BASE_DIR}/service/qtdbushook.cpp"
  "${SRC_BASE_DIR}/service/qtdbushook.h"
  "${SRC_BASE_DIR}/policy/policy.cpp"
  "${SRC_BASE_DIR}/policy/policy.h"
)
add_library(${LIB_NAME} SHARED
  ${LIB_SRCS}
)
target_include_directories(${LIB_NAME} PUBLIC
  Qt5::Core
  Qt5::DBus
  PkgConfig::Systemd # sdbus
  ${SRC_BASE_DIR}
)

target_link_libraries(${LIB_NAME} PRIVATE
  Qt5::Core
  Qt5::DBus
  PkgConfig::Systemd # sdbus
)
ADD_DEFINITIONS(-DEXPORT_LIBRARY)

# TODO install
