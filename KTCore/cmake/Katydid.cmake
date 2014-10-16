# Author: N. Oblath

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})

# Include the build script
include (PackageBuilder)

set (KATYDID_CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

##########
# MACROS #
##########

# This should be called immediately after setting the project name
macro (katydid_prepare_project VERSION_MAJOR VERSION_MINOR REVISION)
	pbuilder_prepare_project (${VERSION_MAJOR} ${VERSION_MINOR} ${REVISION})

    # Add a "Standard" build type
	#set (CMAKE_BUILD_TYPE standard)
	set (CMAKE_CXX_FLAGS_STANDARD "-O1 -DNDEBUG -DSTANDARD" CACHE STRING "Flags used by the compiler during standard builds.")
	set (CMAKE_C_FLAGS_STANDARD "-O1 -DNDEBUG -DSTANDARD" CACHE STRING "Flags used by the compiler during standard builds.")
	set (CMAKE_EXE_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
	set (CMAKE_MODULE_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
	set (CMAKE_SHARED_LINKER_FLAGS_STANDARD "" CACHE STRING "Flags used by the linker during standard builds.")
	mark_as_advanced(CMAKE_CXX_FLAGS_STANDARD CMAKE_C_FLAGS_STANDARD CMAKE_EXE_LINKER_FLAGS_STANDARD CMAKE_MODULE_LINKER_FLAGS_STANDARD CMAKE_SHARED_LINKER_FLAGS_STANDARD)

	# Single-threading option
	set (Katydid_SINGLETHREADED FALSE CACHE BOOL "Flag for running in single-threaded mode")
	if (Katydid_SINGLETHREADED)
	    add_definitions(-DSINGLETHREADED)
	else (Katydid_SINGLETHREADED)
	    remove_definitions(-DSINGLETHREADED)
	endif(Katydid_SINGLETHREADED)

	# Optional compiler flags
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

	# External packages
	include_directories (BEFORE ${KATYDID_CORE_DIR}/External/RapidJSON)
	include_directories (BEFORE ${KATYDID_CORE_DIR}/External/RapidXML)

	# Katydid core directories
	include_directories (BEFORE 
	    ${KATYDID_CORE_DIR}/Utility
	    ${KATYDID_CORE_DIR}/Data
	    ${KATYDID_CORE_DIR}/Processor
	    ${KATYDID_CORE_DIR}/Application
    )
endmacro ()

macro (katydid_build_core_library)
	add_subdirectory (KTCore)
endmacro ()

macro (katydid_build_core_executables)
	add_subdirectory (KTCore/Executables/Main)
	add_subdirectory (KTCore/Executables/Validation)
endmacro ()

