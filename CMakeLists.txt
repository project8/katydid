# CMakeLists for Katydid
# Author: N. Oblath

cmake_minimum_required (VERSION 2.8.12)


# Define the project
project (Katydid)

# Import Katydid core features
include (KTCore/cmake/KatydidCore.cmake)
katydid_prepare_project (2 0 0)

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

#optional build switches
set (Katydid_USE_MANTIS FALSE CACHE BOOL "Flag to optionally use Mantis (external dependency)")
set (Katydid_USE_MONARCH TRUE CACHE BOOL "Flag to optionally use Monarch")
set (Katydid_USE_FFTW TRUE CACHE BOOL "Flag to optionally use FFTW")
set (Katydid_USE_HDF5 TRUE CACHE BOOL "Flag to optionally use HDF5")
set (Katydid_USE_EIGEN FALSE CACHE BOOL "Flag to optionally use eigen")
set (Katydid_USE_ROOT TRUE CACHE BOOL "Flag to optionally use ROOT")
set (Katydid_USE_MATLAB FALSE CACHE BOOL "Flag to optionally use Matlab libraries, needed to read MAT files")


##############
# Dependencies
##############

# Mantis
if (Katydid_USE_MANTIS)
    set (Mantis_PREFIX "" CACHE PATH "Mantis install prefix")
    if (NOT IS_DIRECTORY ${Mantis_PREFIX})
        message (FATAL_ERROR "Please specify the Mantis prefix directory as \"Mantis_PREFIX\" (<${Mantis_PREFIX}> is not a valid directory)")
    endif (NOT IS_DIRECTORY ${Mantis_PREFIX})
    list (APPEND Mantis_LIBRARY_DIRS ${Mantis_PREFIX}/lib)
    list (APPEND Mantis_INCLUDE_DIRS ${Mantis_PREFIX}/include)
    list (APPEND Mantis_LIBRARIES MantisClient MantisServer MantisProto boost_atomic)
    include_directories (${Mantis_INCLUDE_DIRS})
    link_directories (${Mantis_LIBRARY_DIRS})
    pbuilder_add_ext_libraries (${Mantis_LIBRARIES})
    add_definitions (-DUSE_MANTIS)
    set (Mantis_FOUND TRUE)
endif (Katydid_USE_MANTIS)

# Boost (1.46 required for filesystem version 3)
find_package (Boost 1.46.0 REQUIRED COMPONENTS date_time filesystem program_options system thread)
# make sure dynamic linking is assumed for all boost libraries
add_definitions (-DBOOST_ALL_DYN_LINK)
include_directories (${Boost_INCLUDE_DIRS})
pbuilder_add_ext_libraries (${Boost_LIBRARIES})

# FFTW
if (Katydid_USE_FFTW)
    find_package(FFTW REQUIRED)
else (Katydid_USE_FFTW)
    set (FFTW_FOUND FALSE)
endif (Katydid_USE_FFTW)
if (FFTW_FOUND)
    add_definitions(-DFFTW_FOUND)
    pbuilder_add_ext_libraries (${FFTW_LIBRARIES})
    if (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)
        set (FFTW_NTHREADS 1 CACHE STRING "Number of threads to use for FFTW processes")
        add_definitions (-DFFTW_NTHREADS=${FFTW_NTHREADS})
        message (STATUS "FFTW configured to use up to ${FFTW_NTHREADS} threads.")
    else (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)
        remove_definitions (-DFFTW_NTHREADS=${FFTW_NTHREADS})
    endif (FFTW_THREADS_FOUND AND NOT Katydid_SINGLETHREADED)
else (FFTW_FOUND)
    message(STATUS "Building without FFTW")
    remove_definitions(-DFFTW_FOUND)
    remove_definitions (-DFFTW_NTHREADS=${FFTW_NTHREADS})
    set (FFTW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/External/FFTW)
endif (FFTW_FOUND)
include_directories (${FFTW_INCLUDE_DIR})

# HDF5
if (Katydid_USE_HDF5)
    find_package (HDF5 COMPONENTS CXX)
else (Katydid_USE_HDF5)
    set (HDF5_FOUND FALSE)
endif (Katydid_USE_HDF5)
if (HDF5_FOUND)
    #add_definitions (-DHDF5_FOUND)
    include_directories (${HDF5_INCLUDE_DIR})
    pbuilder_add_ext_libraries (${HDF5_CXX_LIBRARIES})
else (HDF5_FOUND)
    #remove_definitions (-DHDF5_FOUND)
    message (STATUS "Building without HDF5")
endif (HDF5_FOUND)

# ROOT
if (Katydid_USE_ROOT)
    find_package (ROOT 5.28)
else (Katydid_USE_ROOT)
    set (ROOT_FOUND FALSE)
endif (Katydid_USE_ROOT)
if (ROOT_FOUND)
    add_definitions(-DROOT_FOUND)
    list (APPEND EXTRA_ROOT_LIBRARIES libHistPainter)
    foreach (lib ${EXTRA_ROOT_LIBRARIES})
        string (STRIP ${lib} lib)
        set (lib "${ROOT_LIBRARY_DIR}/${lib}.so")
        list (APPEND ROOT_LIBRARIES ${lib})
    endforeach (lib)
    pbuilder_add_ext_libraries (${ROOT_LIBRARIES})
    #message(STATUS "${ROOT_LIBRARIES}")
else (ROOT_FOUND)
    message(STATUS "Building without ROOT")
    remove_definitions(-DROOT_FOUND)
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDES})

# Matlab
if (Katydid_USE_MATLAB)
    find_package(Matlab)
endif (Katydid_USE_MATLAB)
if (MATLAB_FOUND)
    message (STATUS "matlab include dir: ${MATLAB_INCLUDE_DIR}")
    get_filename_component (MATLAB_LIBRARY_DIR ${MATLAB_MAT_LIBRARY} DIRECTORY)
    message (STATUS "matlab library dir: ${MATLAB_LIBRARY_DIR}")
    message (STATUS "matlab libraries: ${MATLAB_LIBRARIES}")
    add_definitions(-DUSE_MATLAB)
    include_directories (${MATLAB_INCLUDE_DIR})
    link_directories (${MATLAB_LIBRARY_DIR})
    set (EXTERNAL_LIBRARIES_MATLAB ${MATLAB_LIBRARIES})
    #pbuilder_add_ext_libraries (${MATLAB_LIBRARIES})
else (MATLAB_FOUND)
    message (STATUS "Building without Matlab")
    remove_definitions(-DUSE_MATLAB)
endif (MATLAB_FOUND)

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
#find_package (OpenMP)
if (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-DUSE_OPENMP)
else (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)
    remove_definitions(-DUSE_OPENMP)
endif (OPENMP_FOUND AND NOT Katydid_SINGLETHREADED)


# External packages distributed with Katydid
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/External/nanoflann)

# add an option to build the validation tests
option (Katydid_ENABLE_TESTING "Build the validation tests and enable CMake testing" OFF)
if (Katydid_ENABLE_TESTING)
    enable_testing ()
endif (Katydid_ENABLE_TESTING)

option (Katydid_ENABLE_TUTORIAL "Build the tutorial code" OFF)
if (Katydid_ENABLE_TUTORIAL)
    include_directories (${PROJECT_SOURCE_DIR}/Examples/Tutorial)
endif (Katydid_ENABLE_TUTORIAL)

# add an option to build the profiling tests
option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)

# Monarch
if (Katydid_USE_MONARCH)
    add_subdirectory (Source/Time/Monarch)
    pbuilder_add_submodule_libraries (${Monarch_LIBRARIES})
    add_definitions(-DUSE_MONARCH)

    # add to the RPATH to be used when installing, but only if it's not a system directory
    list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${Monarch_LIBRARY_DIR}" isSystemDir)
    if ("${isSystemDir}" STREQUAL "-1")
       list (APPEND CMAKE_INSTALL_RPATH ${Monarch_LIBRARY_DIR})
    endif ("${isSystemDir}" STREQUAL "-1")
else (Katydid_USE_MONARCH)
    message(STATUS "Building without Monarch")
    add_subdirectory(Source/Time/Monarch EXCLUDE_FROM_ALL)
    remove_definitions(-DUSE_MONARCH)
endif (Katydid_USE_MONARCH)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/Source)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/libthorax)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/libthorax/cpp)

# Subdirectories
include_directories (BEFORE 
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Data/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/EventAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/Time
    ${PROJECT_SOURCE_DIR}/Source/Data/Transform
    ${PROJECT_SOURCE_DIR}/Source/Data/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/IO
    ${PROJECT_SOURCE_DIR}/Source/IO/Conversions
    ${PROJECT_SOURCE_DIR}/Source/Time
    ${PROJECT_SOURCE_DIR}/Source/Simulation
    ${PROJECT_SOURCE_DIR}/Source/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/Transform
    ${PROJECT_SOURCE_DIR}/Source/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/EventAnalysis
)

katydid_build_core_library()

add_subdirectory (Source/Utility)
add_subdirectory (Source/Data)
add_subdirectory (Source/IO)
add_subdirectory (Source/Time)
add_subdirectory (Source/Simulation)
add_subdirectory (Source/Evaluation)
add_subdirectory (Source/Transform)
add_subdirectory (Source/SpectrumAnalysis)
add_subdirectory (Source/EventAnalysis)
add_subdirectory (Source/Executables/Main)
add_subdirectory (Source/Executables/Profiling)
add_subdirectory (Source/Executables/Validation)
add_subdirectory (Examples)

katydid_build_core_executables()

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
