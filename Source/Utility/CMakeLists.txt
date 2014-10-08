# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

set (UTILITY_DICT_HEADERFILES
    KTEventLoop.hh
    KTRootGuiLoop.hh
)

set (UTILITY_NODICT_HEADERFILES
    complexpolar.hh
    KTAxisProperties_GetNBins.hh
    KTAxisProperties.hh
    KTCountHistogram.hh
    KTCutable.hh
    KTCutableArray.hh
    KTECDF.hh
    KTMaskedArray.hh
    KTMath.hh
    KTPhysicalArray.hh
    KTSmooth.hh
    KTSpline.hh
    KTTIFactory.hh
)

set (UTILITY_HEADERFILES ${UTILITY_DICT_HEADERFILES} ${UTILITY_NODICT_HEADERFILES})

set (UTILITY_HEADER_SUBDIRS
    complexpolar
)

set (UTILITY_SOURCEFILES
    complexpolar/specialization.cc
    KTAxisProperties.cc
    KTCountHistogram.cc
    KTEventLoop.cc
    KTException.cc
    KTLogger.cc
    KTParam.cc
    KTParamInputJSON.cc
    KTSmooth.cc
    KTSpline.cc
    KTECDF.cc
)

if (ROOT_FOUND)
    set (UTILITY_SOURCEFILES
        ${CORE_SOURCEFILES}
        KTRootGuiLoop.cc
    )

    set (UTILITY_LINKDEF_HEADERFILE LinkDef/UtilityLinkDef.hh)
    set (UTILITY_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict.cxx)
endif (ROOT_FOUND)

set (KATYDID_LIBS
    Katydid
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY ("${UTILITY_DICT_HEADERFILES}" "${UTILITY_LINKDEF_HEADERFILE}" "${UTILITY_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
endif (ROOT_FOUND)

add_library (KatydidUtility ${UTILITY_SOURCEFILES} ${UTILITY_DICT_OUTFILE})
target_link_libraries (KatydidUtility ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${UTILITY_HEADERFILES})
pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
pbuilder_install_libraries (KatydidUtility)

