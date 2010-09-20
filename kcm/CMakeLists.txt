project(kcmwicd)
cmake_minimum_required(VERSION 2.6)

find_package(KDE4 4.3.0 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

########### next target ###############

kde4_add_ui_files(kcm_wicd_UI_SRCS kcmwidget.ui)
kde4_add_plugin(kcm_wicd wicdkcm.cpp wicddbusinterface.cpp ${kcm_wicd_UI_SRCS})

target_link_libraries(kcm_wicd  ${KDE4_KDECORE_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_SOLID_LIBS} )

install(TARGETS kcm_wicd  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### next target ###############

#kde4_install_auth_actions(org.kde.kcontrol.kcmwicd kcmwicd.actions)

########### install files ###############

install( FILES kcm_wicd.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

#set( kcm_wicd_SRCS wicdkcm.cpp preferencespanel.cpp dbushandler.cpp labelentry.cpp global.cpp)
#kde4_add_plugin( kcm_wicd ${kcm_wicd_SRCS} )
#target_link_libraries( kcm_wicd ${KDE4_KDEUI_LIBS} ${QT_QTNETWORK_LIBRARY} )
#install( TARGETS kcm_wicd  DESTINATION ${PLUGIN_INSTALL_DIR} )
#install( FILES kcm_wicd.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
