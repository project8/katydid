# CMakeLists for Katydid/Source/Analysis
# Author: N. Oblath

set (ANALYSIS_HEADERFILES
    KTAmplitudeCounter.hh
    KTAmplitudeDistributor.hh
    KTConsensusThresholding.hh
    KTCorrelator.hh
    KTCreateKDTree.hh
    KTDataAccumulator.hh
    KTDistanceClustering.hh
    KTGainVariationProcessor.hh
    KTHoughTransform.hh
    KTMergeKDTree.hh
    KTSpectrumDiscriminator.hh
    KTSwitchFFTWPolar.hh
    #KTAutoCorrMatrix.hh
)

set (ANALYSIS_SOURCEFILES
    KTAmplitudeCounter.cc
    KTAmplitudeDistributor.cc
    KTConsensusThresholding.cc
    KTCorrelator.cc
    KTCreateKDTree.cc
    KTDataAccumulator.cc
    KTDistanceClustering.cc
    KTGainVariationProcessor.cc
    KTHoughTransform.cc
    KTMergeKDTree.cc
    KTSpectrumDiscriminator.cc
    KTSwitchFFTWPolar.cc
    #KTAutoCorrMatrix.cc
)

if (FFTW_FOUND)
    set (ANALYSIS_HEADERFILES
        ${ANALYSIS_HEADERFILES}
        KTAnalyticAssociator.hh
        KTWignerVille.hh
    )
    set (ANALYSIS_SOURCEFILES
        ${ANALYSIS_SOURCEFILES}
        KTAnalyticAssociator.cc
        KTWignerVille.cc
    )
endif (FFTW_FOUND)

if (ROOT_FOUND)
    set (ANALYSIS_HEADERFILES 
        ${ANALYSIS_HEADERFILES}   
        KTGainNormalization.hh
        KTVariableSpectrumDiscriminator.hh
    )
    set (ANALYSIS_SOURCEFILES
        ${ANALYSIS_SOURCEFILES}
        KTGainNormalization.cc
        KTVariableSpectrumDiscriminator.cc
    )
endif (ROOT_FOUND)

if (EIGEN3_FOUND)
   set (ANALYSIS_HEADERFILES 
       ${ANALYSIS_HEADERFILES}   
       KTBiasedACM.hh
       KTRQProcessor.hh
   )
   set (ANALYSIS_SOURCEFILES
       ${ANALYSIS_SOURCEFILES}
       KTRQProcessor.cc
       KTBiasedACM.cc
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
