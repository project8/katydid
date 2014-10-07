# CMakeLists for Katydid/Source/Data
# Author: N. Oblath

set (DATA_HEADERFILES
    KTApplyCut.hh
    KTData.hh
    KTCut.hh
    KTCutResult.hh
    KTCutStatus.hh
    KTPrintDataStructure.hh
    Analysis/KTAmplitudeDistribution.hh
    Analysis/KTAnalyticAssociateData.hh
    Analysis/KTCluster1DData.hh
    Analysis/KTCorrelationData.hh
    Analysis/KTCorrelationTSData.hh
    Analysis/KTDiscriminatedPoints1DData.hh
    Analysis/KTDiscriminatedPoints2DData.hh
    Analysis/KTGainVariationData.hh
    Analysis/KTHoughData.hh
    Analysis/KTKDTree.hh
    Analysis/KTKDTreeData.hh
    Analysis/KTNormalizedFSData.hh
    Analysis/KTPointCloud.hh
    Analysis/KTTimeSeriesDist.hh
    Analysis/KTTimeSeriesDistData.hh
    Analysis/KTWV2DData.hh
    Analysis/KTWignerVilleData.hh
    Candidates/KTFrequencyCandidate.hh
    Candidates/KTFrequencyCandidateData.hh
    Candidates/KTMultiTrackEventData.hh
    Candidates/KTProcessedTrackData.hh
    Candidates/KTSparseWaterfallCandidateData.hh
    Candidates/KTWaterfallCandidateData.hh
    Egg/KTDigitizerTestData.hh
    Egg/KTEggHeader.hh
    Egg/KTPhysicalArrayFFTW.hh
    Egg/KTProcSummary.hh
    Egg/KTRawTimeSeries.hh
    Egg/KTRawTimeSeriesData.hh
    Egg/KTSliceHeader.hh
    Egg/KTTimeSeries.hh
    Egg/KTTimeSeriesData.hh
    Egg/KTTimeSeriesFFTW.hh
    Egg/KTTimeSeriesReal.hh
    Evaluation/KTAnalysisCandidates.hh
    Evaluation/KTCCResults.hh
    Evaluation/KTMCTruthEvents.hh
    FFT/KTFrequencySpectrum.hh
    FFT/KTFrequencySpectrumDataFFTW.hh
    FFT/KTFrequencySpectrumDataPolar.hh
    FFT/KTFrequencySpectrumFFTW.hh
    FFT/KTFrequencySpectrumPolar.hh
    FFT/KTPowerSpectrum.hh
    FFT/KTPowerSpectrumData.hh
    FFT/KTMultiFSDataFFTW.hh
    FFT/KTMultiFSDataPolar.hh
    FFT/KTTimeFrequency.hh
    FFT/KTTimeFrequencyDataPolar.hh
    FFT/KTTimeFrequencyPolar.hh
)

set (DATA_SOURCEFILES
    KTApplyCut.cc
    KTData.cc
    KTCut.cc
    KTCutStatus.cc
    KTPrintDataStructure.cc
    Analysis/KTAmplitudeDistribution.cc
    Analysis/KTAnalyticAssociateData.cc
    Analysis/KTCluster1DData.cc
    Analysis/KTCorrelationData.cc
    Analysis/KTCorrelationTSData.cc
    Analysis/KTDiscriminatedPoints1DData.cc
    Analysis/KTDiscriminatedPoints2DData.cc
    Analysis/KTGainVariationData.cc
    Analysis/KTHoughData.cc
    Analysis/KTKDTreeData.cc
    Analysis/KTNormalizedFSData.cc
    Analysis/KTTimeSeriesDist.cc
    Analysis/KTTimeSeriesDistData.cc
    Analysis/KTWV2DData.cc
    Analysis/KTWignerVilleData.cc
    Candidates/KTFrequencyCandidate.cc
    Candidates/KTFrequencyCandidateData.cc
    Candidates/KTMultiTrackEventData.cc
    Candidates/KTProcessedTrackData.cc
    Candidates/KTSparseWaterfallCandidateData.cc
    Candidates/KTWaterfallCandidateData.cc
    Egg/KTDigitizerTestData.cc
    Egg/KTEggHeader.cc
    Egg/KTPhysicalArrayFFTW.cc
    Egg/KTProcSummary.cc
    Egg/KTRawTimeSeries.cc
    Egg/KTRawTimeSeriesData.cc
    Egg/KTSliceHeader.cc
    Egg/KTTimeSeries.cc
    Egg/KTTimeSeriesData.cc
    Egg/KTTimeSeriesFFTW.cc
    Egg/KTTimeSeriesReal.cc
    Evaluation/KTAnalysisCandidates.cc
    Evaluation/KTCCResults.cc
    Evaluation/KTMCTruthEvents.cc
    FFT/KTFrequencySpectrum.cc
    FFT/KTFrequencySpectrumDataFFTW.cc
    FFT/KTFrequencySpectrumDataPolar.cc
    FFT/KTFrequencySpectrumFFTW.cc
    FFT/KTFrequencySpectrumPolar.cc
    FFT/KTMultiFSDataFFTW.cc
    FFT/KTMultiFSDataPolar.cc
    FFT/KTPowerSpectrum.cc
    FFT/KTPowerSpectrumData.cc
    FFT/KTTimeFrequency.cc
    FFT/KTTimeFrequencyDataPolar.cc
    FFT/KTTimeFrequencyPolar.cc
)

if (NOT FFTW_FOUND)
    set (DATA_SOURCEFILES
        ${DATA_SOURCEFILES}
        ${PROJECT_SOURCE_DIR}/External/FFTW/FFTWStandIn.cc
    )
endif (NOT FFTW_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
)

##################################################

add_library (KatydidData ${DATA_SOURCEFILES})
target_link_libraries (KatydidData ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidData)
pbuilder_install_headers(${DATA_HEADERFILES})
