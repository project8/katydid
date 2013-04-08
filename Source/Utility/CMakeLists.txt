# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

if (LOG4CXX_FOUND)
    set (LOGGER_CONFIGURATION "${CONFIG_INSTALL_DIR}/log4cxx.properties" )
    set_source_files_properties (KTLogger.cc PROPERTIES COMPILE_DEFINITIONS
        "LOG4CXX;LOGGER_CONFIGURATION=\"${LOGGER_CONFIGURATION}\""
    )
endif (LOG4CXX_FOUND)

set (UTILITY_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties_GetNBins.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTConcurrentQueue.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutableArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDestroyer.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTExtensibleStruct.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMaskedArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMath.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSingleton.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpline.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTECDF.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTIFactory.hh
)

set (UTILITY_HEADER_SUBDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar
    ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx
)

set (UTILITY_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar/specialization.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx/KTLevel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTECDF.cc
)

##################################################

add_library (KatydidUtility ${UTILITY_SOURCEFILES})
target_link_libraries (KatydidUtility ${LIBS})

pbuilder_install_headers (${UTILITY_HEADERFILES})
pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
pbuilder_install_libraries (KatydidUtility)

# install the log4cxx config file
set (LEVEL "info")
if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set (LEVEL "debug")
endif (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set (LEVEL "prog#KTLevel")
endif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx/config/log4cxx.properties.in ${CONFIG_INSTALL_DIR}/log4cxx.properties @ONLY )

