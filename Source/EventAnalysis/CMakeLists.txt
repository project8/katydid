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
    KTIterativeTrackClustering.hh
    KTLinearFitCut.hh
    KTMultiPeakEventBuilder.hh
    KTMultiPeakTrackProcessing.hh
    KTMultiPeakTrackBuilder.hh
    KTMultiPeakTrackEventSequenceIDCut.hh
    KTMultiPeakTrackMultiplicityCut.hh
    KTMultiSliceClustering.hh
    KTOverlappingTrackClustering.hh
    KTPowerFitCut.hh
    KTProcessedTrackCut.hh
    KTQuadraticPhaseShift.hh
    KTSidebandCorrection.hh
    KTSpectrogramCollector.hh
    KTTrackMainbandCut.hh
    KTTrackMVACut.hh
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
    KTIterativeTrackClustering.cc
    KTLinearFitCut.cc
    KTMultiPeakEventBuilder.cc
    KTMultiPeakTrackProcessing.cc
    KTMultiPeakTrackBuilder.cc
    KTMultiPeakTrackEventSequenceIDCut.cc
    KTMultiPeakTrackMultiplicityCut.cc
    KTMultiSliceClustering.cc
    KTOverlappingTrackClustering.cc
    KTPowerFitCut.cc
    KTProcessedTrackCut.cc
    KTQuadraticPhaseShift.cc
    KTSidebandCorrection.cc
    KTSpectrogramCollector.cc
    KTTrackMainbandCut.cc
    KTTrackMVACut.cc
    KTTrackProcessing.cc
    KTTrackFreqCut.cc
    KTTrackTimeCut.cc
)

if (ROOT_FOUND)
    set (EVENTANALYSIS_HEADERFILES 
        ${EVENTANALYSIS_HEADERFILES}   
        KTLinearDensityProbeFit.hh
        KTTMVAClassifier.hh
    )
    set (EVENTANALYSIS_SOURCEFILES
        ${EVENTANALYSIS_SOURCEFILES}
        KTLinearDensityProbeFit.cc
        KTTMVAClassifier.cc
    )
endif (ROOT_FOUND)

if (DLIB_FOUND)
    set (EVENTANALYSIS_HEADERFILES
        ${EVENTANALYSIS_HEADERFILES}
        KTDLIBClassifier.hh
    )
    set (EVENTANALYSIS_SOURCEFILES
        ${EVENTANALYSIS_SOURCEFILES}
        KTDLIBClassifier.cc
    )
endif (DLIB_FOUND)

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
