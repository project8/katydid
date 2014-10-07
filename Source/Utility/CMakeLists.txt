# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

set (UTILITY_HEADERFILES
    complexpolar.hh
    KTAxisProperties_GetNBins.hh
    KTAxisProperties.hh
    KTConcurrentQueue.hh
    KTCountHistogram.hh
    KTCutable.hh
    KTCutableArray.hh
    KTDestroyer.hh
    KTException.hh
    KTExtensibleStruct.hh
    KTExtensibleStructFactory.hh
    KTFactory.hh
    KTLogger.hh
    KTMaskedArray.hh
    KTMath.hh
    KTMemberVariable.hh
    KTNOFactory.hh
    KTParam.hh
    KTParamInputJSON.hh
    KTPhysicalArray.hh
    KTSingleton.hh
    KTSmooth.hh
    KTSpline.hh
    KTECDF.hh
    KTTIFactory.hh
)

set (UTILITY_HEADER_SUBDIRS
    complexpolar
)

set (UTILITY_SOURCEFILES
    complexpolar/specialization.cc
    KTAxisProperties.cc
    KTCountHistogram.cc
    KTException.cc
    KTLogger.cc
    KTParam.cc
    KTParamInputJSON.cc
    KTSmooth.cc
    KTSpline.cc
    KTECDF.cc
)

##################################################

add_library (KatydidUtility ${UTILITY_SOURCEFILES})
target_link_libraries (KatydidUtility ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${UTILITY_HEADERFILES})
pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
pbuilder_install_libraries (KatydidUtility)

