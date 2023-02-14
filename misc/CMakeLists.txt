macro(install_symlink filepath wantsdir)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link/${wantsdir}/)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/lib/systemd/system/${filepath} ${PROJECT_BINARY_DIR}/link/${wantsdir}/${filepath})
    install(FILES ${PROJECT_BINARY_DIR}/link/${wantsdir}/${filepath} DESTINATION lib/systemd/system/${wantsdir}/)
endmacro(install_symlink)
macro(install_user_symlink filepath wantsdir)
    string(LENGTH ${filepath} file_length)
    math(EXPR begin_pos "${file_length} - 9")
    string(SUBSTRING ${filepath} 0 ${begin_pos} file_name)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link/${wantsdir}/)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/lib/systemd/user/${filepath} ${PROJECT_BINARY_DIR}/link/${wantsdir}/${filepath})
    install(FILES ${PROJECT_BINARY_DIR}/link/${wantsdir}/${filepath} DESTINATION lib/systemd/user/${wantsdir}/)
endmacro(install_user_symlink)

install(FILES org.deepin.service.manager.conf
    DESTINATION share/dbus-1/system.d/
)

install(FILES deepin-service-manager-system.service
    DESTINATION lib/systemd/system/
)

install(FILES deepin-service-plugin-system@.service
    DESTINATION lib/systemd/system/
)

install(FILES deepin-service-manager-user.service
    DESTINATION lib/systemd/user/
)

install(FILES deepin-service-plugin-user@.service
    DESTINATION lib/systemd/user/
)

install_symlink(deepin-service-manager-system.service multi-user.target.wants)
install_user_symlink(deepin-service-manager-user.service default.target.wants)

set(LIB_NAME "deepin-qdbus-service")

# config pkgconfig file
configure_file(${LIB_NAME}.pc.in ${LIB_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
# config cmake file
configure_file(${LIB_NAME}Config.cmake.in ${LIB_NAME}Config.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME})
