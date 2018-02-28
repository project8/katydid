# CMakeLists for Katydid
# Author: N. Oblath

# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required (VERSION 3.1)

# Define the project
cmake_policy( SET CMP0048 NEW ) # version in project()
project (Katydid VERSION 2.6.4)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Nymph/Scarab/cmake )
include( PackageBuilder )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Nymph/cmake )
include( Nymph )

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

pbuilder_prepare_project()


#########
# Options
#########

set_option( USE_CPP11 TRUE )

option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)
option (Katydid_ENABLE_TESTING "Build the validation tests and enable CMake testing" OFF)
option (Katydid_ENABLE_TUTORIAL "Build the tutorial code" OFF)
option (Katydid_USE_MANTIS "Flag to optionally use Mantis (external dependency)" OFF)
option (Katydid_USE_MONARCH "Flag to optionally use Monarch" ON)
option (Katydid_USE_FFTW "Flag to optionally use FFTW" ON)
option (Katydid_USE_HDF5 "Flag to optionally use HDF5" ON)
option (Katydid_USE_EIGEN "Flag to optionally use eigen" OFF)
option (Katydid_USE_ROOT "Flag to optionally use ROOT" ON)
option (Katydid_USE_MATLAB "Flag to optionally use MatIO libraries, needed to read MAT files" ON)
option (Katydid_USE_DLIB "Flag to optionally use DLIB library, required for classifier" OFF)
option (Katydid_BUILD_DOC "Enable the options to build the documentation (requires Doxygen and Sphinx (optional) with Breathe)" OFF)
option (Katydid_USE_SPHINX "Flag to optionally use Sphinx documentation" OFF)

set_option( Scarab_BUILD_CODEC_YAML TRUE )
set_option( Nymph_BUILD_NYMPH_EXE FALSE )


##############
# Dependencies
##############

# Midge
pbuilder_add_submodule( Nymph Nymph )
nymph_process_options()

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
    find_package (ROOT 5.28 COMPONENTS Gui Spectrum TMVA)
else (Katydid_USE_ROOT)
    set (ROOT_FOUND FALSE)
endif (Katydid_USE_ROOT)
if (ROOT_FOUND)
    add_definitions(-DROOT_FOUND)
    pbuilder_add_ext_libraries (${ROOT_LIBRARIES})
    #message(STATUS "${ROOT_LIBRARIES}")
else (ROOT_FOUND)
    message(STATUS "Building without ROOT")
    remove_definitions(-DROOT_FOUND)
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDE_DIR})

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

# dlib
if (Katydid_USE_DLIB)
    find_package(DLIB)
endif (Katydid_USE_DLIB)
if (DLIB_FOUND)
    message (STATUS "DLIB include dir: ${DLIB_INCLUDE_DIR}")
    message (STATUS "DLIB libraries: ${DLIB_LIBRARIES}")
    add_definitions(-DUSE_DLIB)
    include_directories (${DLIB_INCLUDE_DIR})
    pbuilder_add_ext_libraries (${DLIB_LIBRARIES})
else (DLIB_FOUND)
    message (STATUS "Building without DLIB")
    remove_definitions(-DUSE_DLIB)
endif (DLIB_FOUND)

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
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/External/RapidXML)

#######
# Build
#######

if (Katydid_ENABLE_TESTING)
    enable_testing ()
endif (Katydid_ENABLE_TESTING)

if (Katydid_ENABLE_TUTORIAL)
    include_directories (${PROJECT_SOURCE_DIR}/Examples/Tutorial)
    add_definitions (-DENABLE_TUTORIAL)
else (Katydid_ENABLE_TUTORIAL)
    remove_definitions (-DENABLE_TUTORIAL)
endif (Katydid_ENABLE_TUTORIAL)

# Monarch
if (Katydid_USE_MONARCH)
    pbuilder_add_submodule( Monarch Source/Time/Monarch )
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
else (Katydid_USE_MONARCH)
    message(STATUS "Building without Monarch")
    add_subdirectory (Source/Time/Monarch EXCLUDE_FROM_ALL)
    remove_definitions (-DUSE_MONARCH)
    remove_definitions (-DUSE_MONARCH2)
    remove_definitions (-DUSE_MONARCH3)
endif (Katydid_USE_MONARCH)

# Subdirectories
include_directories (BEFORE 
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Data/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/EventAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/Time
    ${PROJECT_SOURCE_DIR}/Source/Data/Transform
    #${PROJECT_SOURCE_DIR}/Source/Data/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/IO
    ${PROJECT_SOURCE_DIR}/Source/IO/Conversions
    ${PROJECT_SOURCE_DIR}/Source/Time
    #${PROJECT_SOURCE_DIR}/Source/Simulation
    ${PROJECT_SOURCE_DIR}/Source/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/Transform
    ${PROJECT_SOURCE_DIR}/Source/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/EventAnalysis
)

add_subdirectory (Source/Utility)
add_subdirectory (Source/Data)
add_subdirectory (Source/IO)
add_subdirectory (Source/Time)
#add_subdirectory (Source/Simulation)
#add_subdirectory (Source/Evaluation)
add_subdirectory (Source/Transform)
add_subdirectory (Source/SpectrumAnalysis)
add_subdirectory (Source/EventAnalysis)
if (Katydid_ENABLE_EXECUTABLES)
    add_subdirectory (Source/Executables/Main)
    add_subdirectory (Source/Executables/Profiling)
    add_subdirectory (Source/Executables/Validation)
endif (Katydid_ENABLE_EXECUTABLES)
add_subdirectory (Examples)
add_subdirectory(../dlib dlib_build)

# Build the main executables (from Nymph, but linked to all of the Katydid libraries)
nymph_build_executables()

# Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
# By default it builds in the source tree.
if (Katydid_BUILD_DOC)
    add_subdirectory (Documentation)
endif (Katydid_BUILD_DOC)

# Extract compile definitions to pass to config files
get_directory_property (Katydid_COMP_DEFS COMPILE_DEFINITIONS)

# Install config files
pbuilder_install_config_files()

# Install the example CMakeLists file for building with this installation of Katydid
configure_file (${PROJECT_SOURCE_DIR}/Examples/CustomApplications/CMakeLists.txt.in ${CMAKE_INSTALL_PREFIX}/example/CMakeLists.txt @ONLY)
pbuilder_install_files (${CMAKE_INSTALL_PREFIX}/example ${PROJECT_SOURCE_DIR}/Examples/CustomApplications/MyApplication.cc)
