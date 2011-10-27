set(wicd_engine_SRCS
    ../dbushandler.cpp
    wicdjob.cpp
    wicdservice.cpp
    wicdengine.cpp
)

kde4_add_plugin(plasma_engine_wicd ${wicd_engine_SRCS})
target_link_libraries(plasma_engine_wicd ${QT_QTNETWORK_LIBRARY} ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_engine_wicd DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-wicd.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES wicd.operations DESTINATION ${DATA_INSTALL_DIR}/plasma/services)

