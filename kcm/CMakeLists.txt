include (KDE4Defaults)

kde4_add_ui_files(kcm_wicd_UI_SRCS kcmwidget.ui)
kde4_add_plugin(kcm_wicd wicdkcm.cpp wicddbusinterface.cpp ${kcm_wicd_UI_SRCS})

target_link_libraries(kcm_wicd ${KDE4_KDEUI_LIBS} ${KDE4_SOLID_LIBS} )

install(TARGETS kcm_wicd  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_wicd.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
