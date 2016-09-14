# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

set (UTILITY_DICT_HEADERFILES
    KTRootGuiLoop.hh
)

set (UTILITY_NODICT_HEADERFILES
    complexpolar.hh
    KTAxisProperties_GetNBins.hh
    KTAxisProperties.hh
    KTConstants.hh
    KTCountHistogram.hh
    KTCutable.hh
    KTCutableArray.hh
    KTECDF.hh
    KTKatydidApp.hh
    KTMaskedArray.hh
    KTMath.hh
    KTPhysicalArray.hh
    KTRandom.hh
    KTSmooth.hh
    KTSpline.hh
    KTVarTypePhysicalArray.hh
)

set (UTILITY_HEADERFILES ${UTILITY_DICT_HEADERFILES} ${UTILITY_NODICT_HEADERFILES})

set (UTILITY_HEADER_SUBDIRS
    complexpolar
)

set (UTILITY_SOURCEFILES
    complexpolar/specialization.cc
    KTAxisProperties.cc
    KTCountHistogram.cc
    KTECDF.cc
    KTKatydidApp.cc
    KTRandom.cc
    KTSmooth.cc
    KTSpline.cc
)

if (ROOT_FOUND)
    set (UTILITY_SOURCEFILES
        ${UTILITY_SOURCEFILES}
        KTRootGuiLoop.cc
    )

    set (UTILITY_LINKDEF_HEADERFILE LinkDef/UtilityLinkDef.hh)
    set (UTILITY_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict.cxx)
    set (UTILITY_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict_rdict.pcm)
endif (ROOT_FOUND)

set (KATYDID_LIBS
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY (UtilityDict ${UTILITY_DICT_HEADERFILES} LINKDEF ${UTILITY_LINKDEF_HEADERFILE})
    if (Katydid_USE_CPP11)
        pbuilder_install_files (${LIB_INSTALL_DIR} ${UTILITY_DICT_PCMFILE})
    endif (Katydid_USE_CPP11)
endif (ROOT_FOUND)

add_library (KatydidUtility ${UTILITY_SOURCEFILES} ${UTILITY_DICT_OUTFILE})
target_link_libraries (KatydidUtility ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${UTILITY_HEADERFILES})
pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
pbuilder_install_libraries (KatydidUtility)

