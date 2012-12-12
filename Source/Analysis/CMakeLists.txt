# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_DICT_HEADERFILES
)

set (ANALYSIS_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.hh
)

set (ANALYSIS_HEADERFILES ${ANALYSIS_DICT_HEADERFILES} ${ANALYSIS_NODICT_HEADERFILES})

set (ANALYSIS_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.cc
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
target_link_libraries (KatydidAnalysis ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidAnalysis)
katydid_install_headers(${ANALYSIS_HEADERFILES})
