# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOldGainNormalization.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOldSimpleClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSwitchFFTWPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.hh
)

set (ANALYSIS_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTFFTEHunt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOldGainNormalization.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOldSimpleClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSwitchFFTWPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.cc
)

if (ROOT_FOUND)
    set (ANALYSIS_HEADERFILES 
        ${ANALYSIS_HEADERFILES}   
        ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.hh
    )
    set (ANALYSIS_SOURCEFILES
        ${ANALYSIS_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.cc
    )
endif (ROOT_FOUND)

if (EIGEN3_FOUND)
   set (ANALYSIS_HEADERFILES 
   ${ANALYSIS_HEADERFILES}   
   #${CMAKE_CURRENT_SOURCE_DIR}/KTBiasedACM.hh
   #${CMAKE_CURRENT_SOURCE_DIR}/KTRQProcessor.hh
   )
   set (ANALYSIS_SOURCEFILES
   ${ANALYSIS_SOURCEFILES}
   #${CMAKE_CURRENT_SOURCE_DIR}/KTRQProcessor.cc
   #${CMAKE_CURRENT_SOURCE_DIR}/KTBiasedACM.cc
   )
endif (EIGEN3_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
    KatydidOutput
    KatydidFFT
)

##################################################

add_library (KatydidAnalysis ${ANALYSIS_SOURCEFILES})
target_link_libraries (KatydidAnalysis ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidAnalysis)
katydid_install_headers(${ANALYSIS_HEADERFILES})
