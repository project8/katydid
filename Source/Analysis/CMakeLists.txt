# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_DICT_HEADERFILES
)

set (ANALYSIS_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHuntProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalizationProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClusteringProcessor.hh
)

set (ANALYSIS_HEADERFILES ${ANALYSIS_DICT_HEADERFILES} ${ANALYSIS_NODICT_HEADERFILES})

set (ANALYSIS_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHuntProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalizationProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClusteringProcessor.cc
)

#set (ANALYSIS_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/AnalysisLinkDef.hh)
#set (ANALYSIS_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/AnalysisDict.cxx)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidEgg
    KatydidFFT
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${ANALYSIS_DICT_HEADERFILES}" "${ANALYSIS_LINKDEF_HEADERFILE}" "${ANALYSIS_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidAnalysis ${ANALYSIS_SOURCEFILES} ${ANALYSIS_DICT_OUTFILE})
set_target_properties(KatydidAnalysis PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
target_link_libraries (KatydidAnalysis ${KATYDID_LIBS} ${LIBS})

install (FILES ${ANALYSIS_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
install (TARGETS KatydidAnalysis  DESTINATION ${LIB_INSTALL_DIR})
         
