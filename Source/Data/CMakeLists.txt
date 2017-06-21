# CMakeLists for Katydid/Source/Data
# Author: N. Oblath

set (DATA_HEADERFILES
    SpectrumAnalysis/KTAmplitudeDistribution.hh
    SpectrumAnalysis/KTAnalyticAssociateData.hh
    SpectrumAnalysis/KTCluster1DData.hh
    SpectrumAnalysis/KTCorrelationData.hh
    SpectrumAnalysis/KTCorrelationTSData.hh
    SpectrumAnalysis/KTDiscriminatedPoints1DData.hh
    SpectrumAnalysis/KTDiscriminatedPoints2DData.hh
    SpectrumAnalysis/KTGainVarChi2Data.hh
    SpectrumAnalysis/KTGainVariationData.hh
    SpectrumAnalysis/KTHoughData.hh
    SpectrumAnalysis/KTKDTree.hh
    SpectrumAnalysis/KTKDTreeData.hh
    SpectrumAnalysis/KTLinearFitResult.hh
    SpectrumAnalysis/KTNormalizedFSData.hh
    SpectrumAnalysis/KTPointCloud.hh
    SpectrumAnalysis/KTTimeSeriesDist.hh
    SpectrumAnalysis/KTTimeSeriesDistData.hh
    SpectrumAnalysis/KTWV2DData.hh
    SpectrumAnalysis/KTWignerVilleData.hh
    SpectrumAnalysis/KTSeqLine.hh
    EventAnalysis/KTFrequencyCandidate.hh
    EventAnalysis/KTFrequencyCandidateData.hh
    EventAnalysis/KTMultiTrackEventData.hh
    EventAnalysis/KTProcessedTrackData.hh
    EventAnalysis/KTSparseWaterfallCandidateData.hh
    EventAnalysis/KTSpectrumCollectionData.hh
    EventAnalysis/KTWaterfallCandidateData.hh
    Time/KTDigitizerTestData.hh
    Time/KTEggHeader.hh
    Time/KTPhysicalArrayFFTW.hh
    Time/KTProcSummary.hh
    Time/KTRawTimeSeries.hh
    Time/KTRawTimeSeriesData.hh
    Time/KTSliceHeader.hh
    Time/KTTimeSeries.hh
    Time/KTTimeSeriesData.hh
    Time/KTTimeSeriesFFTW.hh
    Time/KTTimeSeriesReal.hh
    Evaluation/KTAnalysisCandidates.hh
    Evaluation/KTCCResults.hh
    Evaluation/KTMCTruthEvents.hh
    Transform/KTFrequencyDomainArray.hh
    Transform/KTFrequencySpectrum.hh
    Transform/KTFrequencySpectrumDataFFTW.hh
    Transform/KTFrequencySpectrumDataPolar.hh
    Transform/KTFrequencySpectrumFFTW.hh
    Transform/KTFrequencySpectrumPolar.hh
    Transform/KTPowerSpectrum.hh
    Transform/KTPowerSpectrumData.hh
    Transform/KTPowerSpectrumUncertaintyData.hh
    Transform/KTMultiFSDataFFTW.hh
    Transform/KTMultiFSDataPolar.hh
    Transform/KTTimeFrequency.hh
    Transform/KTTimeFrequencyDataPolar.hh
    Transform/KTTimeFrequencyPolar.hh
    Transform/KTScoredSpectrum.hh
    Transform/KTScoredSpectrumData.hh
)

set (DATA_SOURCEFILES
    SpectrumAnalysis/KTAmplitudeDistribution.cc
    SpectrumAnalysis/KTAnalyticAssociateData.cc
    SpectrumAnalysis/KTCluster1DData.cc
    SpectrumAnalysis/KTCorrelationData.cc
    SpectrumAnalysis/KTCorrelationTSData.cc
    SpectrumAnalysis/KTDiscriminatedPoints1DData.cc
    SpectrumAnalysis/KTDiscriminatedPoints2DData.cc
    SpectrumAnalysis/KTGainVarChi2Data.cc
    SpectrumAnalysis/KTGainVariationData.cc
    SpectrumAnalysis/KTHoughData.cc
    SpectrumAnalysis/KTKDTreeData.cc
    SpectrumAnalysis/KTLinearFitResult.cc
    SpectrumAnalysis/KTNormalizedFSData.cc
    SpectrumAnalysis/KTTimeSeriesDist.cc
    SpectrumAnalysis/KTTimeSeriesDistData.cc
    SpectrumAnalysis/KTWV2DData.cc
    SpectrumAnalysis/KTWignerVilleData.cc
    SpectrumAnalysis/KTSeqLine.cc
    EventAnalysis/KTFrequencyCandidate.cc
    EventAnalysis/KTFrequencyCandidateData.cc
    EventAnalysis/KTMultiTrackEventData.cc
    EventAnalysis/KTProcessedTrackData.cc
    EventAnalysis/KTSparseWaterfallCandidateData.cc
    EventAnalysis/KTSpectrumCollectionData.cc
    EventAnalysis/KTWaterfallCandidateData.cc
    Time/KTDigitizerTestData.cc
    Time/KTEggHeader.cc
    Time/KTPhysicalArrayFFTW.cc
    Time/KTProcSummary.cc
    Time/KTRawTimeSeries.cc
    Time/KTRawTimeSeriesData.cc
    Time/KTSliceHeader.cc
    Time/KTTimeSeries.cc
    Time/KTTimeSeriesData.cc
    Time/KTTimeSeriesFFTW.cc
    Time/KTTimeSeriesReal.cc
    Evaluation/KTAnalysisCandidates.cc
    Evaluation/KTCCResults.cc
    Evaluation/KTMCTruthEvents.cc
    Transform/KTFrequencyDomainArray.cc
    Transform/KTFrequencySpectrum.cc
    Transform/KTFrequencySpectrumDataFFTW.cc
    Transform/KTFrequencySpectrumDataPolar.cc
    Transform/KTFrequencySpectrumFFTW.cc
    Transform/KTFrequencySpectrumPolar.cc
    Transform/KTMultiFSDataFFTW.cc
    Transform/KTMultiFSDataPolar.cc
    Transform/KTPowerSpectrum.cc
    Transform/KTPowerSpectrumData.cc
    Transform/KTPowerSpectrumUncertaintyData.cc
    Transform/KTTimeFrequency.cc
    Transform/KTTimeFrequencyDataPolar.cc
    Transform/KTTimeFrequencyPolar.cc
    Transform/KTScoredSpectrum.cc
    Transform/KTScoredSpectrumData.cc
)

if (NOT FFTW_FOUND)
    set (DATA_SOURCEFILES
        ${DATA_SOURCEFILES}
        ${PROJECT_SOURCE_DIR}/External/FFTW/FFTWStandIn.cc
    )
endif (NOT FFTW_FOUND)

if (Katydid_ENABLE_TUTORIAL)
    set (DATA_HEADERFILES 
        ${DATA_HEADERFILES}
        ${PROJECT_SOURCE_DIR}/Examples/Tutorial/KTLowPassFilteredData.hh
    )
    set (DATA_SOURCEFILES 
        ${DATA_SOURCEFILES}
        ${PROJECT_SOURCE_DIR}/Examples/Tutorial/KTLowPassFilteredData.cc
    )
endif (Katydid_ENABLE_TUTORIAL)


set (KATYDID_LIBS
    KatydidUtility
)

##################################################

add_library (KatydidData ${DATA_SOURCEFILES})
target_link_libraries (KatydidData ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidData)
pbuilder_install_headers(${DATA_HEADERFILES})
