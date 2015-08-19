cmake_minimum_required(VERSION 2.8.9)
project(wicd-kde)

set(KDE_MIN_VERSION "4.5.0")
find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)

include(KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} )

add_subdirectory( kcm )
add_subdirectory( applet )
add_subdirectory( dataengine )
