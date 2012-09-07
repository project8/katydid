# CMakeLists for Katydid
# Author: N. Oblath

# minimum cmake version 2.6 required by the scripts to get Git information
cmake_minimum_required (VERSION 2.6)

# specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# define the project
project (Katydid)

# get git revision information
include (GetGitRevisionDescription)
git_describe (GIT_REV)

# define the variables to describe the package (will go in the KatydidConfig.hh file)
set (Katydid_VERSION_MAJOR 0)
set (Katydid_VERSION_MINOR 5)
set (Katydid_REVISION 1)
set (Katydid_VERSION "${Katydid_VERSION_MAJOR}.${Katydid_VERSION_MINOR}.${Katydid_REVISION}")
set (Katydid_FULL_VERSION "${Katydid_VERSION_MAJOR}.${Katydid_VERSION_MINOR} (${GIT_REV})")
set (Katydid_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set (Katydid_PACKAGE_STRING "${CMAKE_PROJECT_NAME} ${Katydid_FULL_VERSION}")


# Setup the default install prefix
# This gets set to the binary directory upon first configuring.
# If the user changes the prefix, but leaves the flag OFF, then it will remain as the user specified.
# If the user wants to reset the prefix to the default (i.e. the binary directory), then the flag should be set ON.
if (NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT)
    set (SET_INSTALL_PREFIX_TO_DEFAULT ON)
endif (NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT)
if (SET_INSTALL_PREFIX_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} CACHE PATH "Install prefix" FORCE)
    set (SET_INSTALL_PREFIX_TO_DEFAULT OFF CACHE BOOL "Reset default install path when when configuring" FORCE)
endif (SET_INSTALL_PREFIX_TO_DEFAULT)

# install subdirectories
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install directory for headers")
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Install directory for libraries")
set (BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Install directory for binaries")
set (CONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/config" CACHE PATH "Install directory for config files")

# build shared libraries
set (BUILD_SHARED_LIBS ON)

# add an option to build the validation tests
option (ENABLE_TESTING "Build the validation test and enable CMake testing" OFF)
if (ENABLE_TESTING)
    enable_testing ()
endif (ENABLE_TESTING)

# setup the Boost install
# at the time of this writing, CMake 2.8.6 searched for boost up to 1.46, but boost 1.48 exists, so it must be added
# see FindBoost.cmake for additional information about Boost_ADDITIONAL_VERSIONS
set (Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0" "1.46.1" "1.47" "1.47.0" "1.48" "1.48.0" "1.49" "1.49.0" "1.50" "1.50.0")
find_package (Boost 1.36.0 REQUIRED COMPONENTS program_options)
include_directories (${Boost_INCLUDE_DIRS})
set (LIBS ${LIBS} ${Boost_LIBRARIES})

# setup the ROOT install
find_package (ROOT 5.28)
if (ROOT_FOUND)
    add_definitions(-DROOT_FOUND)
    # have to add the FFTW library because it doesn't automatically get included for some reason
    set (ROOT_LIBRARIES ${ROOT_LIBRARIES} -lFFTW)
    set (LIBS ${LIBS} ${ROOT_LIBRARIES})
else (ROOT_FOUND)
    message(STATUS "Building without ROOT")
    # option for not building with ROOT: need a few include files for file types
    set (ROOT_INCLUDES ${PROJECT_SOURCE_DIR}/External/ROOT)
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDES})

# setup the log4cxx install
find_package (LOG4CXX)
if (LOG4CXX_FOUND)
    message (STATUS "Log4cxx found")
    list (APPEND LIBS ${LOG4CXX_LIBRARY})
    include_directories (${LOG4CXX_INCLUDE_DIRS})
endif (LOG4CXX_FOUND)



# external packages distributed with Katydid
include_directories ("${PROJECT_SOURCE_DIR}/External/RapidXML")
include_directories ("${PROJECT_SOURCE_DIR}/External/CImg")


# configuration header file
configure_file (
    "${PROJECT_SOURCE_DIR}/KatydidConfig.hh.in"
    "${PROJECT_BINARY_DIR}/KatydidConfig.hh"
)


# install location for the configuration header
install (FILES "${PROJECT_BINARY_DIR}/KatydidConfig.hh" DESTINATION include)

# add the binary tree to the search path for include files so that the config file is found
include_directories ("${PROJECT_BINARY_DIR}")

# subdirectories
include_directories (
    ${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/include
    ${PROJECT_BINARY_DIR}/Source/Egg/Monarch
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Core
    ${PROJECT_SOURCE_DIR}/Source/Egg
    ${PROJECT_SOURCE_DIR}/Source/FFT
    ${PROJECT_SOURCE_DIR}/Source/Analysis
    ${PROJECT_SOURCE_DIR}/Source/Waterfall
    ${PROJECT_SOURCE_DIR}/Source/Output
)

add_subdirectory (Source/Egg/Monarch)
add_subdirectory (Source/Utility)
add_subdirectory (Source/Core)
add_subdirectory (Source/Egg)
if (ROOT_FOUND)
    add_subdirectory (Source/FFT)
    add_subdirectory (Source/Analysis)
    add_subdirectory (Source/Waterfall)
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


# this is apparently not the right way to run the link_dylibs script
#execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)

install (CODE "execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)")
