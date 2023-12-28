# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

set (UTILITY_DICT_HEADERFILES
    KTRootGuiLoop.hh
)

set (UTILITY_NODICT_HEADERFILES
    complexpolar.hh
    complexpolar/class.hh
    complexpolar/functions.hh
    complexpolar/operators.hh
    complexpolar/specialization.hh
    KTAxisProperties_GetNBins.hh
    KTAxisProperties.hh
    KTConstants.hh
    KTCountHistogram.hh
    KTCutable.hh
    KTCutableArray.hh
    KTDBSCAN.hh
    KTDemangle.hh
    KTECDF.hh
    KTKatydidApp.hh
    KTMaskedArray.hh
    KTMath.hh
    KTPhysicalArray.hh
    KTPhysicalArrayComplex.hh
    KTRandom.hh
    KTSmooth.hh
    KTSpline.hh
    KTStdComplexFuncs.hh
    KTVarTypePhysicalArray.hh
    # ../../Examples/KTProcessorTemplate.hh
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
    KTPhysicalArrayComplex.cc
    # ../../Examples/KTProcessorTemplate.cc
)

if (ROOT_FOUND)
    set (UTILITY_SOURCEFILES
        ${UTILITY_SOURCEFILES}
        KTRootGuiLoop.cc
    )

    set (UTILITY_LINKDEF_HEADERFILE LinkDef/UtilityLinkDef.hh)
    set (UTILITY_SOURCEFILES ${UTILITY_SOURCEFILES} ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict.cxx)
    set (UTILITY_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict_rdict.pcm)
endif (ROOT_FOUND)

set (KATYDID_LIBS
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY (UtilityDict ${UTILITY_DICT_HEADERFILES} LINKDEF ${UTILITY_LINKDEF_HEADERFILE} OPTIONS -inlineInputHeader)
    pbuilder_install_files (${LIB_INSTALL_DIR} ${UTILITY_DICT_PCMFILE})
endif (ROOT_FOUND)

pbuilder_library( 
    TARGET KatydidUtility 
    SOURCES ${UTILITY_SOURCEFILES}
    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
)

pbuilder_component_install_and_export( 
    COMPONENT Library
    LIBTARGETS KatydidUtility
)

pbuilder_install_headers( ${UTILITY_HEADERFILES} )


##pbuilder_library (KatydidUtility UTILITY_SOURCEFILES "")
#pbuilder_install_headers(${CICADA_HEADERFILES})

#add_library (KatydidUtility ${UTILITY_SOURCEFILES} ${UTILITY_DICT_OUTFILE})
#target_link_libraries (KatydidUtility ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

##pbuilder_install_headers (${UTILITY_HEADERFILES})
##pbuilder_install_header_dirs ("*.hh" ${UTILITY_HEADER_SUBDIRS})
#pbuilder_install_libraries (KatydidUtility)


