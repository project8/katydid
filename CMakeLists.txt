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
set (Katydid_VERSION "${Katydid_VERSION_MAJOR}.${Katydid_VERSION_MINOR}")
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


# setup the ROOT install
find_package (ROOT 5.28 REQUIRED)
include_directories (${ROOT_INCLUDES})
# have to add the FFTW library because it doesn't automatically get included for some reason
set (ROOT_LIBRARIES ${ROOT_LIBRARIES} -lFFTW)
set (LIBS ${LIBS} ${ROOT_LIBRARIES})

# add the mxml library
set (LIBS ${LIBS} -lmxml)


# configuration header file
configure_file (
    "${PROJECT_SOURCE_DIR}/KatydidConfig_cmake.h.in"
    "${PROJECT_BINARY_DIR}/KatydidConfig.hh"
)

# install location for the configuration header
install (FILES "${PROJECT_BINARY_DIR}/KatydidConfig.h" DESTINATION include)

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
add_subdirectory (Source/Main)

# this is apparently not the right way to run the link_dylibs script
#execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib})
