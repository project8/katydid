# CMakeLists for Katydid/Source/SpectrumAnalysis
# Author: N. Oblath

set (SPECTRUMANALYSIS_HEADERFILES
    KTAmplitudeCounter.hh
    KTAmplitudeDistributor.hh
    KTConsensusThresholding.hh
    KTCorrelator.hh
    KTCreateKDTree.hh
    KTDataAccumulator.hh
    KTDistanceClustering.hh
    KTGainVarChi2Test.hh
    KTGainVariationProcessor.hh
    KTHoughTransform.hh
    KTLinearDensityProbeFit.hh
    KTMergeKDTree.hh
    KTNNFilter.hh
    KTSpectrumDiscriminator.hh
    KTSwitchFFTWPolar.hh
    KTVariableSpectrumDiscriminator.hh
    #KTAutoCorrMatrix.hh
)

set (SPECTRUMANALYSIS_SOURCEFILES
    KTAmplitudeCounter.cc
    KTAmplitudeDistributor.cc
    KTConsensusThresholding.cc
    KTCorrelator.cc
    KTCreateKDTree.cc
    KTDataAccumulator.cc
    KTDistanceClustering.cc
    KTGainVarChi2Test.cc
    KTGainVariationProcessor.cc
    KTHoughTransform.cc
    KTLinearDensityProbeFit.cc
    KTMergeKDTree.cc
    KTNNFilter.cc
    KTSpectrumDiscriminator.cc
    KTSwitchFFTWPolar.cc
    KTVariableSpectrumDiscriminator.cc
    #KTAutoCorrMatrix.cc
)

if (FFTW_FOUND)
    set (SPECTRUMANALYSIS_HEADERFILES
        ${SPECTRUMANALYSIS_HEADERFILES}
        KTAnalyticAssociator.hh
        KTWignerVille.hh
    )
    set (SPECTRUMANALYSIS_SOURCEFILES
        ${SPECTRUMANALYSIS_SOURCEFILES}
        KTAnalyticAssociator.cc
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

if (EIGEN3_FOUND)
    set (SPECTRUMANALYSIS_HEADERFILES 
        ${SPECTRUMANALYSIS_HEADERFILES}   
        KTBiasedACM.hh
        KTRQProcessor.hh
    )
    set (SPECTRUMANALYSIS_SOURCEFILES
        ${SPECTRUMANALYSIS_SOURCEFILES}
        KTRQProcessor.cc
        KTBiasedACM.cc
    )
endif (EIGEN3_FOUND)

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
    Nymph
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
