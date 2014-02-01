# CMakeLists for Katydid/Source/Data
# Author: N. Oblath

set (DATA_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTAmplitudeDistribution.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTAnalyticAssociateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCluster1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCorrelationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCorrelationTSData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTDiscriminatedPoints1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTDiscriminatedPoints2DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTGainVariationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTHoughData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTNormalizedFSData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTWV2DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTWignerVilleData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTFrequencyCandidate.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTFrequencyCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTWaterfallCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Conversions/KT2ROOT.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTEggHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTPhysicalArrayFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTProcSummary.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTRawTimeSeries.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTRawTimeSeriesData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTSliceHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeries.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesDist.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesDistData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesReal.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTAnalysisCandidates.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTCCResults.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTMCTruthEvents.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumDataFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumDataPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTPowerSpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTMultiFSDataFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTMultiFSDataPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequency.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequencyDataPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequencyPolar.hh
)

set (DATA_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTAmplitudeDistribution.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTAnalyticAssociateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCluster1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCorrelationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTCorrelationTSData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTDiscriminatedPoints1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTDiscriminatedPoints2DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTGainVariationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTHoughData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTNormalizedFSData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTWV2DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/KTWignerVilleData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTFrequencyCandidate.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTFrequencyCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Candidates/KTWaterfallCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Conversions/KT2ROOT.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTEggHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTPhysicalArrayFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTProcSummary.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTRawTimeSeries.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTRawTimeSeriesData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTSliceHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeries.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesDist.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesDistData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Egg/KTTimeSeriesReal.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTAnalysisCandidates.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTCCResults.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Evaluation/KTMCTruthEvents.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumDataFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumDataPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTFrequencySpectrumPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTMultiFSDataFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTMultiFSDataPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTPowerSpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequency.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequencyDataPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/FFT/KTTimeFrequencyPolar.cc
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
