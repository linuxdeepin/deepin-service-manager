
# set(XSESSIONS
# xsessions/deepin.desktop
# )

# install(FILES ${XSESSIONS} DESTINATION share/xsessions/)


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
