# CMakeLists for Katydid/Source/SpectrumAnalysis
# Author: N. Oblath

set (SPECTRUMANALYSIS_HEADERFILES
    KTAggregatedChannelOptimizer.hh
    KTAmplitudeCounter.hh
    KTAmplitudeDistributor.hh
    KTAxialAggregator.hh
    KTChannelAggregator.hh
    KTConsensusThresholding.hh
    KTCorrelator.hh
    KTCreateKDTree.hh
    KTDataAccumulator.hh
    KTDBSCANNoiseFiltering.hh
    KTDistanceClustering.hh
#    KTGainVarChi2Test.hh
    KTGainVariationProcessor.hh
    KTHoughTransform.hh
    KTMergeKDTree.hh
    KTNNFilter.hh
    KTSequentialTrackFinder.hh
    KTSpectrumDiscriminator.hh
    KTSpectrogramStriper.hh
    KTSwitchFFTWPolar.hh
    KTVariableSpectrumDiscriminator.hh
    #KTAutoCorrMatrix.hh
)

set (SPECTRUMANALYSIS_SOURCEFILES
    KTAggregatedChannelOptimizer.cc
    KTAmplitudeCounter.cc
    KTAmplitudeDistributor.cc
    KTAxialAggregator.cc
    KTChannelAggregator.cc
    KTConsensusThresholding.cc
    KTCorrelator.cc
    KTCreateKDTree.cc
    KTDataAccumulator.cc
    KTDBSCANNoiseFiltering.cc
    KTDistanceClustering.cc
#    KTGainVarChi2Test.cc
    KTGainVariationProcessor.cc
    KTHoughTransform.cc
    KTMergeKDTree.cc
    KTNNFilter.cc
    KTSequentialTrackFinder.cc
    KTSpectrumDiscriminator.cc
    KTSpectrogramStriper.cc
    KTSwitchFFTWPolar.cc
    KTVariableSpectrumDiscriminator.cc
    #KTAutoCorrMatrix.cc
)

if (FFTW_FOUND)
    set (SPECTRUMANALYSIS_HEADERFILES
        ${SPECTRUMANALYSIS_HEADERFILES}
        KTAnalyticAssociator.hh
        KTConvolution.hh
        KTWignerVille.hh
    )
    set (SPECTRUMANALYSIS_SOURCEFILES
        ${SPECTRUMANALYSIS_SOURCEFILES}
        KTAnalyticAssociator.cc
        KTConvolution.cc
        KTWignerVille.cc
    )
endif (FFTW_FOUND)

if (ROOT_FOUND)
    set (SPECTRUMANALYSIS_HEADERFILES 
        ${SPECTRUMANALYSIS_HEADERFILES}   
        KTGainNormalization.hh
        KTVariableSpectrumDiscriminator.hh
    )
    set (SPECTRUMANALYSIS_SOURCEFILES
        ${SPECTRUMANALYSIS_SOURCEFILES}
        KTGainNormalization.cc
        KTVariableSpectrumDiscriminator.cc
    )
endif (ROOT_FOUND)

# Disabled due to compile errors
#if (EIGEN3_FOUND)
#    set (SPECTRUMANALYSIS_HEADERFILES 
#        ${SPECTRUMANALYSIS_HEADERFILES}   
#        KTBiasedACM.hh
#        KTRQProcessor.hh
#    )
#    set (SPECTRUMANALYSIS_SOURCEFILES
#        ${SPECTRUMANALYSIS_SOURCEFILES}
#        KTRQProcessor.cc
#        KTBiasedACM.cc
#    )
#endif (EIGEN3_FOUND)

if (Katydid_ENABLE_TUTORIAL)
    set (SPECTRUMANALYSIS_HEADERFILES 
        ${SPECTRUMANALYSIS_HEADERFILES}
        ${PROJECT_SOURCE_DIR}/Examples/Tutorial/KTLowPassFilter.hh
    )
    set (SPECTRUMANALYSIS_SOURCEFILES 
        ${SPECTRUMANALYSIS_SOURCEFILES}
        ${PROJECT_SOURCE_DIR}/Examples/Tutorial/KTLowPassFilter.cc
    )
endif (Katydid_ENABLE_TUTORIAL)

set (KATYDID_LIBS
    KatydidUtility
    KatydidData
    KatydidIO
    KatydidTransform
)

##################################################

add_library (KatydidSpectrumAnalysis ${SPECTRUMANALYSIS_SOURCEFILES})
target_link_libraries (KatydidSpectrumAnalysis ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidSpectrumAnalysis)
pbuilder_install_headers(${SPECTRUMANALYSIS_HEADERFILES})
