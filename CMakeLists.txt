# CMakeLists for Katydid
# Author: N. Oblath

# Minimum cmake version 2.6 required by the scripts to get Git information
cmake_minimum_required (VERSION 2.6)

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# Define the project
project (Katydid)

# Include the build script
include (KatydidBuild)

# Set the version and associated variables
katydid_prepare_project (0 9 0)



# Boost
# at the time of this writing, CMake 2.8.6 searched for boost up to 1.46, but boost 1.48 exists, so it must be added
# see FindBoost.cmake for additional information about Boost_ADDITIONAL_VERSIONS
set (Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0" "1.46.1" "1.47" "1.47.0" "1.48" "1.48.0" "1.49" "1.49.0" "1.50" "1.50.0" "1.51" "1.51.0")
find_package (Boost 1.36.0 REQUIRED COMPONENTS program_options)
include_directories (${Boost_INCLUDE_DIRS})
set (LIBS ${LIBS} ${Boost_LIBRARIES})

# FFTW
find_package(FFTW REQUIRED)
include_directories (${FFTW_INCLUDE_DIR})
set (LIBS ${LIBS} ${FFTW_LIBRARIES})

# ROOT
find_package (ROOT 5.28)
if (ROOT_FOUND)
    add_definitions(-DROOT_FOUND)
    set (LIBS ${LIBS} ${ROOT_LIBRARIES})
else (ROOT_FOUND)
    message(STATUS "Building without ROOT")
    # option for not building with ROOT: need a few include files for file types
    set (ROOT_INCLUDES ${PROJECT_SOURCE_DIR}/External/ROOT)
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDES})

# log4cxx
find_package (LOG4CXX)
if (LOG4CXX_FOUND)
    message (STATUS "Log4cxx found")
    list (APPEND LIBS ${LOG4CXX_LIBRARY})
    include_directories (${LOG4CXX_INCLUDE_DIRS})
endif (LOG4CXX_FOUND)



# External packages distributed with Katydid
include_directories ("${PROJECT_SOURCE_DIR}/External/RapidXML")
include_directories ("${PROJECT_SOURCE_DIR}/External/CImg")

# Monarch
include_directories (${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/Include)
add_subdirectory (Source/Egg/Monarch)

# Subdirectories
include_directories (
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Core
    ${PROJECT_SOURCE_DIR}/Source/Egg
    ${PROJECT_SOURCE_DIR}/Source/FFT
    ${PROJECT_SOURCE_DIR}/Source/Analysis
    #${PROJECT_SOURCE_DIR}/Source/Waterfall
    ${PROJECT_SOURCE_DIR}/Source/Output
)

add_subdirectory (Source/Utility)
add_subdirectory (Source/Core)
add_subdirectory (Source/Egg)
add_subdirectory (Source/FFT)
add_subdirectory (Source/Analysis)
if (ROOT_FOUND)
    #add_subdirectory (Source/Waterfall)
    add_subdirectory (Source/Output)
endif (ROOT_FOUND)
add_subdirectory (Source/Applications/Main)
add_subdirectory (Source/Applications/Validation)

if (NOT ROOT_FOUND)
    add_subdirectory (External/ROOT)
endif (NOT ROOT_FOUND)

# Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
# By default it builds in the source tree.
add_subdirectory (Documentation/ReferenceGuide)


# Install config files
katydid_install_config_files()

# Install the example CMakeLists file for building with this installation of Katydid
configure_file (${PROJECT_SOURCE_DIR}/Scripts/CMakeLists.txt.in ${CMAKE_INSTALL_PREFIX}/example/CMakeLists.txt @ONLY)
katydid_install_files (${CMAKE_INSTALL_PREFIX}/example ${PROJECT_SOURCE_DIR}/Scripts/MyApplication.cc)

# this is apparently not the right way to run the link_dylibs script
#execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)

install (CODE "execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)")
