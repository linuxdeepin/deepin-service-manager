cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

project(dde-service-manager)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -W -Wall")

add_subdirectory("src/dde-service-manager")
add_subdirectory("src/plugin/clipboard")
# FIX 添加tests后在gerrit上编译失败
# add_subdirectory("tests")

install(FILES misc/com.deepin.dde.ServiceManager.conf
    DESTINATION /etc/dbus-1/system.d/
)

install(FILES misc/dde-service-manager-system.service
    DESTINATION /lib/systemd/system/
)

install(FILES misc/dde-service-manager-session.service
    DESTINATION /lib/systemd/user
)

install(FILES misc/dde-service-manager.desktop
    DESTINATION /usr/share/applications/
)

install(FILES misc/dde-service-manager.desktop
    DESTINATION /etc/xdg/autostart/
)
