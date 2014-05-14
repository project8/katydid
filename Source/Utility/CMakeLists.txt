# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

set (UTILITY_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties_GetNBins.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTConcurrentQueue.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCountHistogram.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutableArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDestroyer.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTException.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTExtensibleStruct.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMaskedArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMath.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTNOFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTParam.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSingleton.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpline.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTECDF.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTIFactory.hh
)

set (UTILITY_HEADER_SUBDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar
)

set (UTILITY_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar/specialization.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCountHistogram.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTException.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTParam.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTECDF.cc
)

##################################################

add_library (KatydidUtility ${UTILITY_SOURCEFILES})
target_link_libraries (KatydidUtility ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${UTILITY_HEADERFILES})
pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
pbuilder_install_libraries (KatydidUtility)

