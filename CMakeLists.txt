cmake_minimum_required (VERSION 2.6)

# project name and version
project (Katydid)
set (Katydid_VERSION_MAJOR 0)
set (Katydid_VERSION_MINOR 5)

# check for the ROOT installation
include(${PROJECT_SOURCE_DIR}/cmake/FindROOT.cmake)

# set the build directory
set (PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/cbuild")

# default install directory set to pwd
set (CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}")

# build shared libraries
set (BUILD_SHARED_LIBS ON)

# change the build type
set (CMAKE_BUILD_TYPE Debug)


# set compiler and linker flags
set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${ROOT_LIBRARIES})


# configuration header file
configure_file (
    "${PROJECT_SOURCE_DIR}/KatydidConfig_cmake.h.in"
    "${PROJECT_BINARY_DIR}/KatydidConfig.h"
)

# install location for the configuration header
install (FILES "${PROJECT_BINARY_DIR}/KatydidConfig.h" DESTINATION include)

# add the binary tree to the search path for include files so that the config file is found
include_directories ("${PROJECT_BINARY_DIR}")

# subdirectories
add_subdirectory (Source/Egg)
add_subdirectory (Source/FFT)
add_subdirectory (Source/Main)

include_directories ("${PROJECT_SOURCE_DIR}/Source/Egg")
include_directories ("${PROJECT_SOURCE_DIR}/SOURCE/FFT")

