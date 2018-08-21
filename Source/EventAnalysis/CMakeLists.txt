# CMakeLists for Katydid/Source/EventAnalysis
# Author: N. Oblath

#set (EVENTANALYSIS_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EventAnalysisLinkDef.hh)
#set (EVENTANALYSIS_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EventAnalysisDict.cxx)

set (EVENTANALYSIS_HEADERFILES
    KTCollinearTrackClustering.hh
    KTDataCutter.hh
    KTDBScan.hh
    KTDistanceMatrix.hh
    KTDBScanEventClustering.hh
    KTDBScanTrackClustering.hh
    CutClasses/KTEventFirstTrackNUPCut.hh
    CutClasses/KTEventFirstTrackPowerCut.hh
    CutClasses/KTEventFirstTrackSNRCut.hh
    CutClasses/KTEventTimeCut.hh
    CutClasses/KTEventTimeInAcqCut.hh
    KTFrequencyCandidateIdentifier.hh
    KTIterativeTrackClustering.hh
    CutClasses/KTLinearFitCut.hh
    KTMultiPeakEventBuilder.hh
    KTMultiPeakTrackProcessing.hh
    KTMultiPeakTrackBuilder.hh
    CutClasses/KTMultiPeakTrackEventSequenceIDCut.hh
    CutClasses/KTMultiPeakTrackMultiplicityCut.hh
    KTMultiSliceClustering.hh
    KTOverlappingTrackClustering.hh
    CutClasses/KTPowerFitCut.hh
    CutClasses/KTProcessedTrackCut.hh
    KTQuadraticPhaseShift.hh
    KTRPClassifier.hh
    CutClasses/KTSequentialLineNUPCut.hh
    CutClasses/KTSequentialLinePointDensityCut.hh
    CutClasses/KTSequentialLineSNRCut.hh
    KTSidebandCorrection.hh
    KTSpectrogramCollector.hh
    CutClasses/KTTrackMainbandCut.hh
    CutClasses/KTTrackMVACut.hh
    KTTrackProcessingDoubleCuts.hh
    KTTrackProcessingWeightedSlope.hh
    CutClasses/KTTrackFreqCut.hh
    CutClasses/KTTrackTimeCut.hh
)

set (EVENTANALYSIS_SOURCEFILES
    KTCollinearTrackClustering.cc
    KTDataCutter.cc
    KTDBScanEventClustering.cc
    KTDBScanTrackClustering.cc
    CutClasses/KTEventFirstTrackNUPCut.cc
    CutClasses/KTEventFirstTrackPowerCut.cc
    CutClasses/KTEventFirstTrackSNRCut.cc
    CutClasses/KTEventTimeCut.cc
    CutClasses/KTEventTimeInAcqCut.cc
    KTFrequencyCandidateIdentifier.cc
    KTIterativeTrackClustering.cc
    CutClasses/KTLinearFitCut.cc
    KTMultiPeakEventBuilder.cc
    KTMultiPeakTrackProcessing.cc
    KTMultiPeakTrackBuilder.cc
    CutClasses/KTMultiPeakTrackEventSequenceIDCut.cc
    CutClasses/KTMultiPeakTrackMultiplicityCut.cc
    KTMultiSliceClustering.cc
    KTOverlappingTrackClustering.cc
    CutClasses/KTPowerFitCut.cc
    CutClasses/KTProcessedTrackCut.cc
    KTRPClassifier.cc
    KTQuadraticPhaseShift.cc
    CutClasses/KTSequentialLineNUPCut.cc
    CutClasses/KTSequentialLinePointDensityCut.cc
    CutClasses/KTSequentialLineSNRCut.cc
    KTSidebandCorrection.cc
    KTSpectrogramCollector.cc
    CutClasses/KTTrackMainbandCut.cc
    CutClasses/KTTrackMVACut.cc
    KTTrackProcessingDoubleCuts.cc
    KTTrackProcessingWeightedSlope.cc
    CutClasses/KTTrackFreqCut.cc
    CutClasses/KTTrackTimeCut.cc
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
