project(wicd-client-kde)

find_package( KDE4 REQUIRED )
include_directories( ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} )

macro_ensure_version("4.3.80" ${KDE_VERSION} KSTATUSNOTIFIERITEM_FOUND)

add_subdirectory( kcm )
add_subdirectory( icons )
add_subdirectory( translations )

set(client_SRCS
    global.cpp
    main.cpp
    mainwindow.cpp
    networkpanel.cpp
    labelentry.cpp
    scriptsdialog.cpp
    networkpropertiesdialog.cpp
    profilemanager.cpp
    infosdialog.cpp
    networkmodel.cpp
    networkitemdelegate.cpp
    trayicon.cpp
    dbushandler.cpp)

kde4_add_ui_files(client_SRCS profilemanager.ui)

kde4_add_executable( wicd-client-kde ${client_SRCS} )
target_link_libraries( wicd-client-kde ${KDE4_KDEUI_LIBS} )
install( TARGETS wicd-client-kde  ${INSTALL_TARGETS_DEFAULT_ARGS} )

kde4_add_executable(wicdclient_scripts_helper  scriptshelper.cpp)
target_link_libraries(wicdclient_scripts_helper ${KDE4_KDECORE_LIBS})
install(TARGETS wicdclient_scripts_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.wicdclient.scripts wicdclient.scripts.actions)
kde4_install_auth_helper_files(wicdclient_scripts_helper org.kde.wicdclient.scripts root)

install( FILES wicd-client-kde.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES wicd-client-kde.desktop  DESTINATION ${AUTOSTART_INSTALL_DIR} )
install( FILES wicd-client-kde.notifyrc DESTINATION ${DATA_INSTALL_DIR}/wicd-client-kde )
