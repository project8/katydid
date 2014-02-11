# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAmplitudeCounter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAmplitudeDistributor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDataAccumulator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSwitchFFTWPolar.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTAutoCorrMatrix.hh
)

set (ANALYSIS_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAmplitudeCounter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAmplitudeDistributor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDataAccumulator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDistanceClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughTransform.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSpectrumDiscriminator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSwitchFFTWPolar.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTAutoCorrMatrix.cc
)

if (FFTW_FOUND)
    set (ANALYSIS_HEADERFILES
        ${ANALYSIS_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.hh
    )
    set (ANALYSIS_SOURCEFILES
        ${ANALYSIS_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociator.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVille.cc
    )
endif (FFTW_FOUND)

if (ROOT_FOUND)
    set (ANALYSIS_HEADERFILES 
        ${ANALYSIS_HEADERFILES}   
        ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTVariableSpectrumDiscriminator.hh
    )
    set (ANALYSIS_SOURCEFILES
        ${ANALYSIS_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTGainNormalization.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTVariableSpectrumDiscriminator.cc
    )
endif (ROOT_FOUND)

if (EIGEN3_FOUND)
   set (ANALYSIS_HEADERFILES 
   ${ANALYSIS_HEADERFILES}   
   ${CMAKE_CURRENT_SOURCE_DIR}/KTBiasedACM.hh
   ${CMAKE_CURRENT_SOURCE_DIR}/KTRQProcessor.hh
   )
   set (ANALYSIS_SOURCEFILES
   ${ANALYSIS_SOURCEFILES}
   ${CMAKE_CURRENT_SOURCE_DIR}/KTRQProcessor.cc
   ${CMAKE_CURRENT_SOURCE_DIR}/KTBiasedACM.cc
   )
endif (EIGEN3_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
    KatydidIO
    KatydidFFT
)

##################################################

add_library (KatydidAnalysis ${ANALYSIS_SOURCEFILES})
target_link_libraries (KatydidAnalysis ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidAnalysis)
pbuilder_install_headers(${ANALYSIS_HEADERFILES})
