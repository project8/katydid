# CMakeLists for Katydid
# Author: N. Oblath

# Minimum cmake verison 3.6 required for CMake 3.27 and higher (3.5 and below is deprecated)
cmake_minimum_required (VERSION 3.6)


# Define the project
cmake_policy( SET CMP0048 NEW ) # version in project()
project (Katydid VERSION 2.20.3)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Nymph/Scarab/cmake )
include( PackageBuilder )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Nymph/cmake )
include( Nymph )

include( PythonPackage )

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

pbuilder_prepare_project()


#########
# Options
#########

# require C++17
set_option( USE_CPP17 TRUE )

option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)
option (Katydid_ENABLE_TESTING "Build the validation tests and enable CMake testing" OFF)
option (Katydid_ENABLE_TUTORIAL "Build the tutorial code" OFF)
option (Katydid_USE_MONARCH "Flag to optionally use Monarch" ON)
option (Katydid_USE_PYTHON "Flag to optionally build python bindings" OFF)
option (Katydid_USE_MATLAB "Flag to optionally use MatIO libraries, needed to read MAT files" ON)
option (Katydid_USE_FFTW "Flag to optionally use FFTW" ON)
option (Katydid_USE_ROOT "Flag to optionally use ROOT" ON)
option (Katydid_USE_HDF5 "Flag to optionally use HDF5" ON)
option (Katydid_USE_MAGICKPP "Flag to optionally use GraphicsMagick and the Magick++ API" OFF)
option (Katydid_USE_MANTIS "Flag to optionally use Mantis (external dependency)" OFF)
option (Katydid_USE_EIGEN "Flag to optionally use eigen" ON)
option (Katydid_USE_DLIB "Flag to optionally use DLIB library, required for classifier" OFF)

set_option( Scarab_BUILD_PARAM TRUE )
set_option( Scarab_BUILD_CODEC_YAML TRUE )
set_option( Scarab_BUILD_CODEC_JSON TRUE )
set_option( Nymph_BUILD_NYMPH_EXE FALSE )
set_option( Cicada_ENABLE_KATYDID_NAMESPACE FALSE )
set_option( Cicada_ADD_CICADA_PY TRUE )
set_option( Cicada_ENABLE_EXECUTABLES FALSE )


##############
# Dependencies
##############

# Nymph
pbuilder_add_submodule( Nymph Nymph )
nymph_process_options()
add_definitions( -DRAPIDJSON_FILE_BUFFER_SIZE=${RAPIDJSON_FILE_BUFFER_SIZE} )

# Cicada
if (Katydid_USE_ROOT)
    pbuilder_add_submodule( Cicada Cicada )
endif (Katydid_USE_ROOT)

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
list (APPEND Boost_COMPONENTS date_time filesystem program_options system thread)
# python optional
if (Katydid_USE_PYTHON)
    list(APPEND Boost_COMPONENTS python)
endif (Katydid_USE_PYTHON)
#find_package (Boost 1.46.0 REQUIRED COMPONENTS date_time filesystem program_options system thread)
find_package (Boost 1.46.0 REQUIRED COMPONENTS ${Boost_COMPONENTS})
# make sure dynamic linking is assumed for all boost libraries
add_definitions (-DBOOST_ALL_DYN_LINK)
include_directories (${Boost_INCLUDE_DIRS})
pbuilder_add_ext_libraries (${Boost_LIBRARIES})

# eigen
# versions earlier than 3.3 may work; the exact minimum version is not known for sure.
find_package (Eigen3 3.3 REQUIRED)
if (EIGEN3_FOUND)
   message (STATUS "Eigen found.")
   include_directories (${EIGEN3_INCLUDE_DIR})
endif (EIGEN3_FOUND)

## TODO Lots of convention questions... BHL
# python stuff is all here, maybe should not be?
if (Katydid_USE_PYTHON)
    python_package_find_python( 2.7 )
    SET( Nymph_ENABLE_PYTHON ON CACHE BOOL "" FORCE)
else (Katydid_USE_PYTHON)
    set( Nymph_ENABLE_PYTHON OFF CACHE BOOL "" FORCE)
endif (Katydid_USE_PYTHON)

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
    find_package (ROOT 6.00 COMPONENTS Gui Spectrum TMVA)
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

# GraphicsMagic & Magick++
if (Katydid_USE_MAGICKPP)
    find_package(GraphicsMagick REQUIRED)
endif (Katydid_USE_MAGICKPP)
if (MAGICK++_FOUND)
    message (STATUS "GraphicsMagick & Magick++ include dirs: ${MAGICK_INCLUDE_DIR} ${MAGICK++_INCLUDE_DIR}")
    message (STATUS "GraphicsMagick & Magick++ libraries: ${MAGICK_LIBRARIES} ${MAGICK++_LIBRARIES}")
    include_directories (${MAGICK_INCLUDE_DIR} ${MAGICK++_INCLUDE_DIR})
    pbuilder_add_ext_libraries (${MAGICK_LIBRARIES} ${MAGICK++_LIBRARIES})
endif (MAGICK++_FOUND)

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
    # we need the definitions for HAS_ATTR_IFC and HAS_GRP_IFC from Monarch to use the proper class interfaces in M3Header and M3Stream
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS ${Monarch_COMPILE_DEFINITIONS} )
    message(STATUS "Monarch compile definitions: ${Monarch_COMPILE_DEFINITIONS}")
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
    ${PROJECT_SOURCE_DIR}/Source/Data
    ${PROJECT_SOURCE_DIR}/Source/Data/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/EventAnalysis
    ${PROJECT_SOURCE_DIR}/Source/Data/Time
    ${PROJECT_SOURCE_DIR}/Source/Data/Transform
    #${PROJECT_SOURCE_DIR}/Source/Data/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/IO
    ${PROJECT_SOURCE_DIR}/Source/IO/BasicAsciiWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/BasicROOTFileWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/Conversions
    ${PROJECT_SOURCE_DIR}/Source/IO/DataDisplay
    ${PROJECT_SOURCE_DIR}/Source/IO/HDF5Writer
    ${PROJECT_SOURCE_DIR}/Source/IO/JSONWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/MultiSliceROOTWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/OfficialCandidatesWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/ROOTSpectrogramWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/ROOTTreeWriter
    ${PROJECT_SOURCE_DIR}/Source/IO/TerminalWriter
    ${PROJECT_SOURCE_DIR}/Source/Time
    #${PROJECT_SOURCE_DIR}/Source/Simulation
    ${PROJECT_SOURCE_DIR}/Source/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/Transform
    ${PROJECT_SOURCE_DIR}/Source/SpectrumAnalysis
    ${PROJECT_SOURCE_DIR}/Source/EventAnalysis
    ${PROJECT_SOURCE_DIR}/Source/EventAnalysis/CutClasses
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

# Build the main executables (from Nymph, but linked to all of the Katydid libraries)
nymph_build_executables()

# Python

if (Katydid_USE_PYTHON)
    python_package_add_module( Source/KatydidPy.cc )
endif (Katydid_USE_PYTHON)

# Extract compile definitions to pass to config files
get_directory_property (Katydid_COMP_DEFS COMPILE_DEFINITIONS)

# Install config files
pbuilder_install_config_files()

# Install the this_katydid_root script
configure_file( this_katydid.sh.in this_katydid.sh )
pbuilder_install_files( ${BIN_INSTALL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/this_katydid.sh )

# Install the example CMakeLists file for building with this installation of Katydid
configure_file (${PROJECT_SOURCE_DIR}/Examples/CustomApplications/CMakeLists.txt.in ${CMAKE_INSTALL_PREFIX}/example/CMakeLists.txt @ONLY)
pbuilder_install_files (${CMAKE_INSTALL_PREFIX}/example ${PROJECT_SOURCE_DIR}/Examples/CustomApplications/MyApplication.cc)
