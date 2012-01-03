# CMakeLists for Katydid
# Author: N. Oblath

# minimum cmake version 2.6 required by the scripts to get Git information
cmake_minimum_required (VERSION 2.6)

# specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


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


# set the build directory
set (PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/cbuild")

# default install directory set to pwd
set (CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}")

# build shared libraries
set (BUILD_SHARED_LIBS ON)

# change the build type
set (CMAKE_BUILD_TYPE "Debug")

# add an option to build the validation tests
option (ENABLE_TESTING "Build the validation test and enable CMake testing" OFF)
if (ENABLE_TESTING)
    enable_testing ()
endif (ENABLE_TESTING)

# setup the Boost install
# at the time of this writing, CMake 2.8.6 searched for boost up to 1.46, but boost 1.48 exists, so it must be added
# see FindBoost.cmake for additional information about Boost_ADDITIONAL_VERSIONS
set (Boost_ADDITIONAL_VERSIONS "1.48" "1.48.0")
find_package (Boost 1.36.0 REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
set (LIBS ${LIBS} ${Boost_LIBRARIES})

# setup the ROOT install
find_package (ROOT 5.28 REQUIRED)
include_directories (${ROOT_INCLUDES})
# have to add the FFTW library because it doesn't automatically get included for some reason
set (ROOT_LIBRARIES ${ROOT_LIBRARIES} -lFFTW)
set (LIBS ${LIBS} ${ROOT_LIBRARIES})

# add the mxml library
#set (LIBS ${LIBS} -lmxml)


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
include_directories ("${PROJECT_SOURCE_DIR}/Source/Utility")
include_directories ("${PROJECT_SOURCE_DIR}/Source/Framework")
include_directories ("${PROJECT_SOURCE_DIR}/Source/Egg")
include_directories ("${PROJECT_SOURCE_DIR}/Source/FFT")

add_subdirectory (Source/Utility)
add_subdirectory (Source/Framework)
add_subdirectory (Source/Egg)
add_subdirectory (Source/FFT)
add_subdirectory (Source/Applications/Main)
add_subdirectory (Source/Applications/Validation)

# Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
# By default it builds in the source tree.
add_subdirectory (Documentation/ReferenceGuide)


# this is apparently not the right way to run the link_dylibs script
#execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)

install (CODE "execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)")
