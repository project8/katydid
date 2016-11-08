# CMakeLists for Katydid/Source/EventAnalysis
# Author: N. Oblath

#set (EVENTANALYSIS_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EventAnalysisLinkDef.hh)
#set (EVENTANALYSIS_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EventAnalysisDict.cxx)

set (EVENTANALYSIS_HEADERFILES
    KTCandidateCleaner.hh
    KTCandidateTrackFinder.hh
    KTDataCutter.hh
    KTDBScan.hh
    KTDistanceMatrix.hh
    KTDBScanEventClustering.hh
    KTDBScanTrackClustering.hh
    KTEventTimeCut.hh
    KTFrequencyCandidateIdentifier.hh
    KTLinearDensityProbeFit.hh
    KTLinearFitCut.hh
    KTMultiPeakEventBuilder.hh
    KTMultiSliceClustering.hh
    KTPowerFitCut.hh
    KTProcessedTrackCut.hh
    KTSidebandCorrection.hh
    KTSpectrogramCollector.hh
    KTTrackProcessing.hh
    KTTrackTimeCut.hh
)

set (EVENTANALYSIS_SOURCEFILES
    KTCandidateCleaner.cc
    KTCandidateTrackFinder.cc
    KTDataCutter.cc
    KTDBScanEventClustering.cc
    KTDBScanTrackClustering.cc
    KTEventTimeCut.cc
    KTFrequencyCandidateIdentifier.cc
    KTLinearDensityProbeFit.cc
    KTLinearFitCut.cc
    KTMultiPeakEventBuilder.cc
    KTMultiSliceClustering.cc
    KTPowerFitCut.cc
    KTProcessedTrackCut.cc
    KTSidebandCorrection.cc
    KTSpectrogramCollector.cc
    KTTrackProcessing.cc
    KTTrackTimeCut.cc
)

set (KATYDID_LIBS
    KatydidUtility
    KatydidData
    KatydidIO
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${EVENTANALYSIS_DICT_HEADERFILES}" "${EVENTANALYSIS_LINKDEF_HEADERFILE}" "${EVENTANALYSIS_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidEventAnalysis ${EVENTANALYSIS_SOURCEFILES})
target_link_libraries (KatydidEventAnalysis ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidEventAnalysis)
pbuilder_install_headers(${EVENTANALYSIS_HEADERFILES})
