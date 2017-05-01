# CMakeLists for Katydid/Source/EventAnalysis
# Author: N. Oblath

#set (EVENTANALYSIS_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EventAnalysisLinkDef.hh)
#set (EVENTANALYSIS_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EventAnalysisDict.cxx)

set (EVENTANALYSIS_HEADERFILES
    KTCandidateCleaner.hh
    KTCandidateTrackFinder.hh
    KTCollinearTrackClustering.hh
    KTDataCutter.hh
    KTDBScan.hh
    KTDistanceMatrix.hh
    KTDBScanEventClustering.hh
    KTDBScanTrackClustering.hh
    KTEventTimeCut.hh
    KTFrequencyCandidateIdentifier.hh
    KTLinearFitCut.hh
    KTMultiPeakEventBuilder.hh
    KTMultiPeakTrackBuilder.hh
    KTMultiPeakTrackEventSequenceIDCut.hh
    KTMultiPeakTrackMultiplicityCut.hh
    KTMultiSliceClustering.hh
    KTPowerFitCut.hh
    KTProcessedTrackCut.hh
    KTSidebandCorrection.hh
    KTSpectrogramCollector.hh
    KTTrackProcessing.hh
    KTTrackFreqCut.hh
    KTTrackTimeCut.hh
)

set (EVENTANALYSIS_SOURCEFILES
    KTCandidateCleaner.cc
    KTCandidateTrackFinder.cc
    KTCollinearTrackClustering.cc
    KTDataCutter.cc
    KTDBScanEventClustering.cc
    KTDBScanTrackClustering.cc
    KTEventTimeCut.cc
    KTFrequencyCandidateIdentifier.cc
    KTLinearFitCut.cc
    KTMultiPeakEventBuilder.cc
    KTMultiPeakTrackBuilder.cc
    KTMultiPeakTrackEventSequenceIDCut.cc
    KTMultiPeakTrackMultiplicityCut.cc
    KTMultiSliceClustering.cc
    KTPowerFitCut.cc
    KTProcessedTrackCut.cc
    KTSidebandCorrection.cc
    KTSpectrogramCollector.cc
    KTTrackProcessing.cc
    KTTrackFreqCut.cc
    KTTrackTimeCut.cc
)

if (ROOT_FOUND)
    set (EVENTANALYSIS_HEADERFILES 
        ${EVENTANALYSIS_HEADERFILES}   
        KTLinearDensityProbeFit.hh
    )
    set (EVENTANALYSIS_SOURCEFILES
        ${EVENTANALYSIS_SOURCEFILES}
        KTLinearDensityProbeFit.cc
    )
endif (ROOT_FOUND)

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
