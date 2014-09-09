# CMakeLists for Katydid
# Author: N. Oblath

cmake_minimum_required (VERSION 2.8)


# Define the project
project (Katydid)

# Specify the local directory for CMake modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

# Include the build script
include (PackageBuilder)

# Set the version and associated variables
pbuilder_prepare_project (1 10 3)

# Add a "Standard" build type
#set (CMAKE_BUILD_TYPE standard)
set (CMAKE_CXX_FLAGS_STANDARD "-O1 -DNDEBUG -DSTANDARD" CACHE STRING "Flags used by the compiler during standard builds.")
set (CMAKE_C_FLAGS_STANDARD "-O1 -DNDEBUG -DSTANDARD" CACHE STRING "Flags used by the compiler during standard builds.")
set (CMAKE_EXE_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
set (CMAKE_MODULE_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
set (CMAKE_SHARED_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
mark_as_advanced(CMAKE_CXX_FLAGS_STANDARD CMAKE_C_FLAGS_STANDARD CMAKE_EXE_LINKER_FLAGS_STANDARD CMAKE_MODULE_LINKER_FLAGS_STANDARD CMAKE_SHARED_LINKER_FLAGS_STANDARD)


set (Katydid_SINGLETHREADED FALSE CACHE BOOL "Flag for running in single-threaded mode")
if (Katydid_SINGLETHREADED)
    add_definitions(-DSINGLETHREADED)
else (Katydid_SINGLETHREADED)
    remove_definitions(-DSINGLETHREADED)
endif(Katydid_SINGLETHREADED)

#optional build switches
set (Katydid_USE_MANTIS FALSE CACHE BOOL "Flag to optionally use Mantis (external dependency)")
set (Katydid_USE_MONARCH TRUE CACHE BOOL "Flag to optionally use Monarch")
set (Katydid_USE_FFTW TRUE CACHE BOOL "Flag to optionally use FFTW")
set (Katydid_USE_EIGEN TRUE CACHE BOOL "Flag to optionally use eigen")
set (Katydid_USE_ROOT TRUE CACHE BOOL "Flag to optionally use ROOT")
set (Katydid_USE_MATLAB FALSE CACHE BOOL "Flag to optionally use Matlab libraries, needed to read MAT files")

#optional compiler flags
set (FLAG_WARNINGS "" CACHE STRING "Extra warning and error related flags")
set (FLAG_WARNING_LEVEL "NORMAL" CACHE STRING "Valid values are NONE(-w), NORMAL(), MORE(-Wall), MOST(-Wall -Wextra)")
set (FLAG_WARN_AS_ERROR FALSE CACHE BOOL "Treat all warnings as errors")
mark_as_advanced(FLAG_WARNINGS)
if (${CMAKE_BUILD_TYPE} MATCHES RELEASE|RelWithDebInfo|MinSizeRel/i)
    set(FLAG_WARN_AS_ERROR TRUE)
endif ()
if (FLAG_WARN_AS_ERROR)
    message(STATUS "warnings as errors")
    set (FLAG_WARNINGS "-Werror ${FLAG_WARNINGS}")
else (FLAG_WARN_AS_ERROR)
    STRING(REGEX REPLACE "-Werror " "_" FLAG_WARNINGS "${FLAG_WARNINGS}")
endif(FLAG_WARN_AS_ERROR)
if (${FLAG_WARNING_LEVEL} MATCHES NONE)
    STRING(REGEX REPLACE "-Wall |-Wextra " "" FLAG_WARNINGS "${FLAG_WARNINGS}")
    set (FLAG_WARNINGS "${FLAG_WARNINGS} -w")
endif (${FLAG_WARNING_LEVEL} MATCHES NONE)
if (${FLAG_WARNING_LEVEL} MATCHES NORMAL)
    STRING(REGEX REPLACE "-Wall |-Wextra |-w" "" FLAG_WARNINGS "${FLAG_WARNINGS}")
endif (${FLAG_WARNING_LEVEL} MATCHES NORMAL)
if (${FLAG_WARNING_LEVEL} MATCHES MORE)
    STRING(REGEX REPLACE "-Wextra |-w" "" FLAG_WARNINGS "${FLAG_WARNINGS}")
    set (FLAG_WARNINGS "-Wall ${FLAG_WARNINGS}")
endif (${FLAG_WARNING_LEVEL} MATCHES MORE)
if (${FLAG_WARNING_LEVEL} MATCHES MOST)
    STRING(REGEX REPLACE "-w" "" FLAG_WARNINGS "${FLAG_WARNINGS}")
    set (FLAG_WARNINGS "-Wall -Wextra ${FLAG_WARNINGS}")
endif (${FLAG_WARNING_LEVEL} MATCHES MOST)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG_WARNINGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG_WARNINGS}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    pbuilder_add_ext_libraries (rt)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

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
    message (STATUS "matlab libraries: ${MATLAB_LIBRARIES}")
    add_definitions(-DUSE_MATLAB)
    include_directories (${MATLAB_INCLUDE_DIR})
    link_directories (${MATLAB_LIBRARY_DIR})
    pbuilder_add_ext_libraries (${MATLAB_LIBRARIES})
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
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/External/RapidJSON)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/External/RapidXML)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/External/nanoflann)

# add an option to build the validation tests
option (Katydid_ENABLE_TESTING "Build the validation tests and enable CMake testing" OFF)
if (Katydid_ENABLE_TESTING)
    enable_testing ()
endif (Katydid_ENABLE_TESTING)

# add an option to build the profiling tests
option (Katydid_ENABLE_PROFILING "Build the profiling tests" OFF)

# Monarch
if (Katydid_USE_MONARCH)
    add_subdirectory (Source/Egg/Monarch)
    pbuilder_add_submodule_libraries (${Monarch_LIBRARIES})
    add_definitions(-DUSE_MONARCH)

    # add to the RPATH to be used when installing, but only if it's not a system directory
    list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${Monarch_LIBRARY_DIR}" isSystemDir)
    if ("${isSystemDir}" STREQUAL "-1")
       list (APPEND CMAKE_INSTALL_RPATH ${Monarch_LIBRARY_DIR})
    endif ("${isSystemDir}" STREQUAL "-1")
else (Katydid_USE_MONARCH)
    message(STATUS "Building without Monarch")
    add_subdirectory(Source/Egg/Monarch EXCLUDE_FROM_ALL)
    remove_definitions(-DUSE_MONARCH)
endif (Katydid_USE_MONARCH)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/Source)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/libthorax)
include_directories (BEFORE ${PROJECT_SOURCE_DIR}/Source/Egg/Monarch/libthorax/cpp)

# Subdirectories
include_directories (BEFORE 
    ${PROJECT_SOURCE_DIR}/Source/Utility
    ${PROJECT_SOURCE_DIR}/Source/Core
    ${PROJECT_SOURCE_DIR}/Source/Data
    ${PROJECT_SOURCE_DIR}/Source/Data/Analysis
    ${PROJECT_SOURCE_DIR}/Source/Data/Candidates
    ${PROJECT_SOURCE_DIR}/Source/Data/Egg
    ${PROJECT_SOURCE_DIR}/Source/Data/FFT
    ${PROJECT_SOURCE_DIR}/Source/Data/Evaluation
    ${PROJECT_SOURCE_DIR}/Source/IO
    ${PROJECT_SOURCE_DIR}/Source/IO/Conversions
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
add_subdirectory (Source/IO)
add_subdirectory (Source/Egg)
add_subdirectory (Source/Simulation)
add_subdirectory (Source/Evaluation)
add_subdirectory (Source/FFT)
add_subdirectory (Source/Analysis)
add_subdirectory (Source/Candidates)
add_subdirectory (Source/Applications/Main)
add_subdirectory (Source/Applications/Profiling)
add_subdirectory (Source/Applications/Validation)

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
