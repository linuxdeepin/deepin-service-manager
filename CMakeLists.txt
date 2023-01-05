cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

project(dde-service-manager)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif()

add_subdirectory("src")
# TODO
# add_subdirectory("misc")

# TODO:tests, 目前的tests是以前的，后续删掉，补充新的test
# add_subdirectory("tests")

