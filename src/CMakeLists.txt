set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-W -Wall -fPIC -fstack-protector-all -z relro -z noexecstack -z now -pie")

set(SRC_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDEEPIN_SERVICE_MANAGER_DIR="${CMAKE_BINARY_DIR}/lib/")
else()
    ADD_DEFINITIONS(-DDEEPIN_SERVICE_MANAGER_DIR="${CMAKE_INSTALL_PREFIX}/share/dde-service-manager/")
endif()

add_subdirectory("dde-service-manager")
add_subdirectory("libddeqdbusservice")

add_subdirectory("plugin-qt")
add_subdirectory("plugin-sdbus")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory("demo")
endif()