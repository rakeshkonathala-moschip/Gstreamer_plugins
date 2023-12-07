cmake_minimum_required(VERSION 3.0)
project(drawshapes)

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)

#finding math library
find_library(MATH_LIBRARY m)
include_directories(${GST_INCLUDE_DIRS}  ${GST_VIDEO_INCLUDE_DIRS} )
link_directories(${GST_LIBRARY_DIRS}  ${GST_VIDEO_LIBRARY_DIRS} )

# Define the macros that were missing
add_definitions(-DPACKAGE_VERSION="1.0.0")  # Replace with your desired version
add_definitions(-DGST_LICENSE="LGPL")
add_definitions(-DGST_PACKAGE_NAME="drawshapes")
add_definitions(-DGST_PACKAGE_ORIGIN="Rakesh Konathala")

add_library(drawshapes SHARED
    gstdrawshapes.c
)

target_link_libraries(drawshapes
    ${GST_LIBRARIES}
    ${GST_VIDEO_LIBRARIES}
    ${MATH_LIBRARY}
    
)

install(
    TARGETS drawshapes
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0
)

