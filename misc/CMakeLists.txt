macro(install_symlink filepath wantsdir arg)
    string(LENGTH ${filepath} file_length)
    math(EXPR begin_pos "${file_length} - 9")
    string(SUBSTRING ${filepath} 0 ${begin_pos} file_name)
    string(CONCAT link_file ${file_name} "@" ${arg} ".service")
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link/${wantsdir}/)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/lib/systemd/system/${filepath} ${PROJECT_BINARY_DIR}/link/${wantsdir}/${link_file})
    install(FILES ${PROJECT_BINARY_DIR}/link/${wantsdir}/${link_file} DESTINATION lib/systemd/system/${wantsdir}/)
endmacro(install_symlink)
macro(install_user_symlink filepath wantsdir arg)
    string(LENGTH ${filepath} file_length)
    math(EXPR begin_pos "${file_length} - 9")
    string(SUBSTRING ${filepath} 0 ${begin_pos} file_name)
    string(CONCAT link_file ${file_name} "@" ${arg} ".service")
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link/${wantsdir}/)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/lib/systemd/user/${filepath} ${PROJECT_BINARY_DIR}/link/${wantsdir}/${link_file})
    install(FILES ${PROJECT_BINARY_DIR}/link/${wantsdir}/${link_file} DESTINATION lib/systemd/user/${wantsdir}/)
endmacro(install_user_symlink)

install(FILES org.deepin.service.manager.conf
    DESTINATION share/dbus-1/system.d/
)

install(FILES deepin-service-manager@.service
    DESTINATION lib/systemd/system/
)

install(FILES deepin-service-manager@.service
    DESTINATION lib/systemd/user/
)

install_symlink(deepin-service-manager@.service multi-user.target.wants system)
install_user_symlink(deepin-service-manager@.service default.target.wants user)
