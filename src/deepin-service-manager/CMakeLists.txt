add_subdirectory(manager)
add_subdirectory(plugin-loader)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/other/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/manager.json ${CMAKE_BINARY_DIR}/share/other/)
else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manager.json DESTINATION share/deepin-service-manager/other/)
endif()

