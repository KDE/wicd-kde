project(wicd-kde)

set(KDE_MIN_VERSION "4.5.0")
find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)

include_directories( ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} )

if (NOT PYTHONBIN)
    set (PYTHONBIN "python" CACHE STRING "python command")
endif (NOT PYTHONBIN)
configure_file (config-python.h.cmake config-python.h)

add_subdirectory( kcm )
add_subdirectory( icons )
add_subdirectory( applet )
add_subdirectory( dataengine )

# set(client_SRCS
#     global.cpp
#     main.cpp
#     mainwindow.cpp
#     networkpanel.cpp
#     labelentry.cpp
#     scriptsdialog.cpp
#     networkpropertiesdialog.cpp
#     profilemanager.cpp
#     infosdialog.cpp
#     networkmodel.cpp
#     networkitemdelegate.cpp
#     trayicon.cpp
#     dbushandler.cpp)

# kde4_add_ui_files(client_SRCS profilemanager.ui)
# kde4_add_kcfg_files(client_SRCS settings.kcfgc)
# install(FILES wicd-kde.kcfg DESTINATION ${KCFG_INSTALL_DIR})

# kde4_add_executable( wicd-kde ${client_SRCS} )
# target_link_libraries( wicd-kde ${KDE4_KDEUI_LIBS} ${QT_QTNETWORK_LIBRARY} ${KDE4_KNOTIFYCONFIG_LIBS})
# install( TARGETS wicd-kde  ${INSTALL_TARGETS_DEFAULT_ARGS} )

kde4_add_executable(wicdclient_scripts_helper  scriptshelper.cpp)
target_link_libraries(wicdclient_scripts_helper ${KDE4_KDECORE_LIBS})
install(TARGETS wicdclient_scripts_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.wicdclient.scripts wicdclient.scripts.actions)
kde4_install_auth_helper_files(wicdclient_scripts_helper org.kde.wicdclient.scripts root)

# install( PROGRAMS wicd-kde.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
#install( FILES wicd-kde.desktop  DESTINATION ${AUTOSTART_INSTALL_DIR} )
# install( FILES wicd-kde.notifyrc wicd-kdeui.rc DESTINATION ${DATA_INSTALL_DIR}/wicd-kde )
