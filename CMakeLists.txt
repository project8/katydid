# CMakeLists for Katydid
# Author: N. Oblath

cmake_minimum_required (VERSION 3.0)


# Define the project
cmake_policy( SET CMP0048 NEW ) # version in project()
project (Katydid VERSION 2.4.2)

# Import Katydid core features
include (Nymph/cmake/Nymph.cmake)
nymph_prepare_project ()

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

#optional build switches
set (Katydid_USE_CPP11 TRUE CACHE BOOL "Flag to optionally use C++11")
set (Katydid_USE_MANTIS FALSE CACHE BOOL "Flag to optionally use Mantis (external dependency)")
set (Katydid_USE_MONARCH TRUE CACHE BOOL "Flag to optionally use Monarch")
set (Katydid_USE_FFTW TRUE CACHE BOOL "Flag to optionally use FFTW")
set (Katydid_USE_HDF5 TRUE CACHE BOOL "Flag to optionally use HDF5")
set (Katydid_USE_EIGEN FALSE CACHE BOOL "Flag to optionally use eigen")
set (Katydid_USE_ROOT TRUE CACHE BOOL "Flag to optionally use ROOT")
set (Katydid_USE_MATLAB TRUE CACHE BOOL "Flag to optionally use MatIO libraries, needed to read MAT files")
set (Katydid_ENABLE_BUILD_DOC FALSE CACHE BOOL "Enable the options to build the documentation (requires Doxygen and Sphinx with Breathe)")

if (Katydid_USE_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (Katydid_USE_CPP11)

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
    include_directories (BEFORE ${HDF5_INCLUDE_DIR})
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
    find_package(MatIO)
endif (Katydid_USE_MATLAB)
if (MATIO_FOUND)
    message (STATUS "MatIO include dirs: ${MATIO_INCLUDE_DIRS}")
    message (STATUS "MatIO libraries: ${MATIO_LIBRARIES}")
    add_definitions(-DUSE_MATLAB)
    include_directories (${MATIO_INCLUDE_DIRS})
    pbuilder_add_ext_libraries (${MATIO_LIBRARIES})
else (MATIO_FOUND)
    message (STATUS "Building without Matlab")
    remove_definitions(-DUSE_MATLAB)
endif (MATIO_FOUND)

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
    add_definitions (-DENABLE_TUTORIAL)
else (Katydid_ENABLE_TUTORIAL)
    remove_definitions (-DENABLE_TUTORIAL)
endif (Katydid_ENABLE_TUTORIAL)

# add an option to build the profiling tests
option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)

# libthorax
add_subdirectory( libthorax )
pbuilder_add_submodule_libraries( ${libthorax_LIBRARIES} )
include_directories( ${libthorax_DEP_INCLUDE_DIRS} )
# add to the RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${libthorax_LIBRARY_DIR}" isSystemDir )
if ( "${isSystemDir}" STREQUAL "-1" )
   list( APPEND CMAKE_INSTALL_RPATH ${libthorax_LIBRARY_DIR} )
endif( "${isSystemDir}" STREQUAL "-1" )

# Monarch
if (Katydid_USE_MONARCH)
    add_subdirectory (Source/Time/Monarch)
    pbuilder_add_submodule_libraries (${Monarch_LIBRARIES})
    add_definitions (-DUSE_MONARCH)
    if (Monarch_BUILD_MONARCH2)
        add_definitions(-DUSE_MONARCH2)
    else (Monarch_BUILD_MONARCH2)
        remove_definitions(-DUSE_MONARCH2)
    endif (Monarch_BUILD_MONARCH2)
    if (Monarch_BUILD_MONARCH3 )
        add_definitions(-DUSE_MONARCH3)
    else (Monarch_BUILD_MONARCH3)
        remove_definitions(-DUSE_MONARCH3)
    endif (Monarch_BUILD_MONARCH3)

    # add to the RPATH to be used when installing, but only if it's not a system directory
    list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${Monarch_LIBRARY_DIR}" isSystemDir)
    if ("${isSystemDir}" STREQUAL "-1")
       list (APPEND CMAKE_INSTALL_RPATH ${Monarch_LIBRARY_DIR})
    endif ("${isSystemDir}" STREQUAL "-1")
else (Katydid_USE_MONARCH)
    message(STATUS "Building without Monarch")
    add_subdirectory (Source/Time/Monarch EXCLUDE_FROM_ALL)
    remove_definitions (-DUSE_MONARCH)
    remove_definitions (-DUSE_MONARCH2)
    remove_definitions (-DUSE_MONARCH3)
endif (Katydid_USE_MONARCH)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/Monarch2)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/Monarch3)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/libthorax)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Time/Monarch/libthorax/cpp)
include_directories (BEFORE ${PROJECT_BINARY_DIR}/Source/Time/Monarch)

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

nymph_build_core_library()

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

nymph_build_core_executables()

# Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
# By default it builds in the source tree.
if (Katydid_ENABLE_BUILD_DOC)
    add_subdirectory (Documentation)
endif (Katydid_ENABLE_BUILD_DOC)

# Extract compile definitions to pass to config files
get_directory_property (Katydid_COMP_DEFS COMPILE_DEFINITIONS)

# Install config files
pbuilder_install_config_files()

# Install the example CMakeLists file for building with this installation of Katydid
configure_file (${PROJECT_SOURCE_DIR}/Examples/CustomApplications/CMakeLists.txt.in ${CMAKE_INSTALL_PREFIX}/example/CMakeLists.txt @ONLY)
pbuilder_install_files (${CMAKE_INSTALL_PREFIX}/example ${PROJECT_SOURCE_DIR}/Examples/CustomApplications/MyApplication.cc)
