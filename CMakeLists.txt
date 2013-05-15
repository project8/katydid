# CMakeLists for Katydid
# Author: N. Oblath

# Minimum cmake version 2.6 required by the scripts to get Git information
cmake_minimum_required (VERSION 2.6)

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# Define the project
project (Katydid)

# Include the build script
include (PackageBuilder)

# Set the version and associated variables
pbuilder_prepare_project (1 0 0)

set (Katydid_SINGLETHREADED FALSE CACHE BOOL "Flag for running in single-threaded mode")
if (Katydid_SINGLETHREADED)
    add_definitions(-DSINGLETHREADED)
else (Katydid_SINGLETHREADED)
    remove_definitions(-DSINGLETHREADED)
endif(Katydid_SINGLETHREADED)

#optional build switches
set (Katydid_USE_EIGEN TRUE CACHE BOOL "Flag to optionally use eigen")
set (Katydid_USE_ROOT TRUE CACHE BOOL "Flag to optionally use ROOT")
set (Katydid_USE_LOG4CXX TRUE CACHE BOOL "Flag to optionally use log4cxx")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list (APPEND LIBS rt)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Boost (1.46 required for filesystem version 3)
find_package (Boost 1.46.0 REQUIRED COMPONENTS date_time filesystem program_options system thread)
include_directories (${Boost_INCLUDE_DIRS})
list (APPEND LIBS ${Boost_LIBRARIES})

# FFTW
find_package(FFTW REQUIRED)
include_directories (${FFTW_INCLUDE_DIR})
list (APPEND LIBS ${FFTW_LIBRARIES})
if (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)
    set (FFTW_NTHREADS 1 CACHE STRING "Number of threads to use for FFTW processes")
    add_definitions (-DFFTW_NTHREADS=${FFTW_NTHREADS})
    message (STATUS "FFTW configured to use up to ${FFTW_NTHREADS} threads.")
else (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)
    remove_definitions (-DFFTW_NTHREADS=${FFTW_NTHREADS})
endif (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)


# ROOT
if (Katydid_USE_ROOT)
    find_package (ROOT 5.28)
else (Katydid_USE_ROOT)
    set (ROOT_FOUND FALSE)
endif (Katydid_USE_ROOT)
if (ROOT_FOUND)
    add_definitions(-DROOT_FOUND)
    list (APPEND LIBS ${ROOT_LIBRARIES})
else (ROOT_FOUND)
    message(STATUS "Building without ROOT")
    # option for not building with ROOT: need a few include files for file types
    set (ROOT_INCLUDES ${PROJECT_SOURCE_DIR}/External/ROOT)
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDES})

# log4cxx
if (Katydid_USE_LOG4CXX)
    find_package (Log4CXX)
else (Katydid_USE_LOG4CXX)
    set (LOG4CXX_FOUND FALSE)
endif (Katydid_USE_LOG4CXX)
if (LOG4CXX_FOUND)
    message (STATUS "Log4cxx found")
    list (APPEND LIBS ${LOG4CXX_LIBRARY})
    include_directories (${LOG4CXX_INCLUDE_DIRS})
endif (LOG4CXX_FOUND)

# eigen
if (Katydid_USE_EIGEN)
    find_package (Eigen3)
else (Katydid_USE_EIGEN)
    set (EIGEN3_FOUND FALSE)
endif (Katydid_USE_EIGEN)
if (EIGEN3_FOUND)
   message (STATUS "Eigen found.")
   include_directories (${EIGEN3_INCLUDE_DIR})
endif (EIGEN3_FOUND)

# OpenMP
find_package (OpenMP)
if (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-DUSE_OPENMP)
else (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)
    remove_definitions(-DUSE_OPENMP)
endif (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)

# External packages distributed with Katydid
include_directories ("${PROJECT_SOURCE_DIR}/External/RapidJSON")
include_directories ("${PROJECT_SOURCE_DIR}/External/RapidXML")

# add an option to build the validation tests
option (Katydid_ENABLE_TESTING "Build the validation tests and enable CMake testing" OFF)
if (Katydid_ENABLE_TESTING)
    enable_testing ()
endif (Katydid_ENABLE_TESTING)

# add an option to build the profiling tests
option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)

# Monarch
include_directories (${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/Include)
add_subdirectory (Source/Egg/Monarch)
list (APPEND LIBS ${Monarch_LIBRARIES})

# add to the RPATH to be used when installing, but only if it's not a system directory
list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${Monarch_LIBRARY_DIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
   list (APPEND CMAKE_INSTALL_RPATH ${Monarch_LIBRARY_DIR})
endif ("${isSystemDir}" STREQUAL "-1")

# Subdirectories
include_directories (
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Core
    ${PROJECT_SOURCE_DIR}/Source/Data/Analysis
    ${PROJECT_SOURCE_DIR}/Source/Data/Candidates
    ${PROJECT_SOURCE_DIR}/Source/Data/Egg
    ${PROJECT_SOURCE_DIR}/Source/Data/FFT
    ${PROJECT_SOURCE_DIR}/Source/Data/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/Input
    ${PROJECT_SOURCE_DIR}/Source/Output
    ${PROJECT_SOURCE_DIR}/Source/Egg
    ${PROJECT_SOURCE_DIR}/Source/Simulation
    ${PROJECT_SOURCE_DIR}/Source/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/FFT
    ${PROJECT_SOURCE_DIR}/Source/Analysis
    ${PROJECT_SOURCE_DIR}/Source/Candidates
)

add_subdirectory (Source/Utility)
add_subdirectory (Source/Core)
add_subdirectory (Source/Data)
add_subdirectory (Source/Input)
add_subdirectory (Source/Output)
add_subdirectory (Source/Egg)
add_subdirectory (Source/Simulation)
add_subdirectory (Source/Evaluation)
add_subdirectory (Source/FFT)
add_subdirectory (Source/Analysis)
add_subdirectory (Source/Candidates)
add_subdirectory (Source/Applications/Main)
add_subdirectory (Source/Applications/Profiling)
add_subdirectory (Source/Applications/Validation)

if (NOT ROOT_FOUND)
    add_subdirectory (External/ROOT)
endif (NOT ROOT_FOUND)

# Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
# By default it builds in the source tree.
add_subdirectory (Documentation/ReferenceGuide)


# Install config files
pbuilder_install_config_files()

# Install the example CMakeLists file for building with this installation of Katydid
configure_file (${PROJECT_SOURCE_DIR}/Scripts/CMakeLists.txt.in ${CMAKE_INSTALL_PREFIX}/example/CMakeLists.txt @ONLY)
pbuilder_install_files (${CMAKE_INSTALL_PREFIX}/example ${PROJECT_SOURCE_DIR}/Scripts/MyApplication.cc)

# this is apparently not the right way to run the link_dylibs script
#execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${PROJECT_BINARY_DIR}/lib)

if (ROOT_FOUND)
    install (CODE "execute_process (COMMAND ${PROJECT_SOURCE_DIR}/Scripts/link_dylibs.sh ${LIB_INSTALL_DIR})")
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Scripts/LoadLibraries.C.in ${PROJECT_BINARY_DIR}/LoadLibraries.C)
    install (FILES ${PROJECT_BINARY_DIR}/LoadLibraries.C DESTINATION ${LIB_INSTALL_DIR})
endif (ROOT_FOUND)
