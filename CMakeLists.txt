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

kde4_add_executable(wicdclient_scripts_helper  scriptshelper.cpp)
target_link_libraries(wicdclient_scripts_helper ${KDE4_KDECORE_LIBS})
install(TARGETS wicdclient_scripts_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.wicdclient.scripts wicdclient.scripts.actions)
kde4_install_auth_helper_files(wicdclient_scripts_helper org.kde.wicdclient.scripts root)
