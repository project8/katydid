# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCluster1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints2DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOldGainNormalization.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.hh
)

set (ANALYSIS_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCluster1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints2DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOldGainNormalization.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.cc
)

if (ROOT_FOUND)
   set (ANALYSIS_HEADERFILES 
   ${ANALYSIS_HEADERFILES}   
   ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.hh)
   set (ANALYSIS_SOURCEFILES
   ${ANALYSIS_SOURCEFILES}
   ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.cc)
endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidOutput
    KatydidEgg
    KatydidFFT
)

##################################################

add_library (KatydidAnalysis ${ANALYSIS_SOURCEFILES})
target_link_libraries (KatydidAnalysis ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidAnalysis)
katydid_install_headers(${ANALYSIS_HEADERFILES})
