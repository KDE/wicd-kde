if (NOT PYTHONBIN)
    set (PYTHONBIN "python" CACHE STRING "python command")
endif (NOT PYTHONBIN)
configure_file (config-python.h.cmake config-python.h)

set(wicdapplet_SRCS 
    global.cpp
    labelentry.cpp
    properties/scriptsdialog.cpp
    properties/networkpropertiesdialog.cpp
    ../dbushandler.cpp
    profilewidget.cpp
    infodialog.cpp
    networkicon.cpp
    networkitem.cpp
    wirednetworkitem.cpp
    wirelessnetworkitem.cpp
    networkview.cpp
    networkplotter.cpp
    wicdapplet.cpp
)
kde4_add_ui_files(wicdapplet_SRCS wicdappletConfig.ui)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_wicd ${wicdapplet_SRCS})
target_link_libraries(plasma_applet_wicd 
                      ${QT_QTNETWORK_LIBRARY} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_wicd
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-wicd.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install( FILES wicd-kde.notifyrc DESTINATION ${DATA_INSTALL_DIR}/wicd-kde )

kde4_add_executable(wicdclient_scripts_helper  properties/scriptshelper.cpp)
target_link_libraries(wicdclient_scripts_helper ${KDE4_KDECORE_LIBS})
install(TARGETS wicdclient_scripts_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.wicdclient.scripts properties/wicdclient.scripts.actions)
kde4_install_auth_helper_files(wicdclient_scripts_helper org.kde.wicdclient.scripts root)
