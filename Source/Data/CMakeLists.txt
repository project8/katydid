# CMakeLists for Katydid/Source/Data
# Author: N. Oblath

set (DATA_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCluster1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints2DData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTNormalizedFSData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArrayFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSliceHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeFrequency.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeFrequencyPolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeries.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesReal.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWaterfallCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVilleData.hh
)

set (DATA_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAnalyticAssociateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCluster1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCorrelationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints1DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDiscriminatedPoints2DData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGainVariationData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHoughData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTNormalizedFSData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArrayFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSliceHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeFrequency.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeFrequencyPolar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeries.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesReal.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWaterfallCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWignerVilleData.cc
)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
)

##################################################

add_library (KatydidData ${DATA_SOURCEFILES})
target_link_libraries (KatydidData ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidData)
katydid_install_headers(${DATA_HEADERFILES})
