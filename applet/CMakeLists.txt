include(KDE4Defaults)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../
		    ${CMAKE_SOURCE_DIR}
		    ${CMAKE_BINARY_DIR}
		    ${CMAKE_CURRENT_BINARY_DIR}/../
		    ${KDE4_INCLUDES}
)
 
# We add our source code here
set(wicdapplet_SRCS 
    ../global.cpp
    ../labelentry.cpp
    ../scriptsdialog.cpp
    ../networkpropertiesdialog.cpp
    ../profilemanager.cpp
    ../dbushandler.cpp
    infosdialog.cpp
    networkicon.cpp
    networkitem.cpp
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
