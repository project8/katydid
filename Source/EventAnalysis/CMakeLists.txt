# CMakeLists for Katydid/Source/EventAnalysis
# Author: N. Oblath

#set (EVENTANALYSIS_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EventAnalysisLinkDef.hh)
#set (EVENTANALYSIS_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EventAnalysisDict.cxx)

set (EVENTANALYSIS_HEADERFILES
    CutClasses/KTEventFirstTrackNUPCut.hh
    CutClasses/KTEventFirstTrackPowerCut.hh
    CutClasses/KTEventFirstTrackSNRCut.hh
    CutClasses/KTEventTimeCut.hh
    CutClasses/KTEventTimeInAcqCut.hh
    CutClasses/KTLinearFitCut.hh
    CutClasses/KTMultiPeakTrackEventSequenceIDCut.hh
    CutClasses/KTMultiPeakTrackMultiplicityCut.hh
    CutClasses/KTPowerFitCut.hh
    CutClasses/KTProcessedTrackCut.hh
    CutClasses/KTSequentialLineNUPCut.hh
    CutClasses/KTSequentialLinePointDensityCut.hh
    CutClasses/KTSequentialLineSNRCut.hh
    CutClasses/KTTrackFreqCut.hh
    CutClasses/KTTrackMainbandCut.hh
    CutClasses/KTTrackMVACut.hh
    CutClasses/KTTrackTimeCut.hh
    KTCollinearTrackClustering.hh
    
    KTDataCutter.hh
    KTDistanceMatrix.hh
    KTDBSCANEventClustering.hh
    KTDBSCANTrackClustering.hh
    KTFrequencyCandidateIdentifier.hh
    KTIterativeTrackClustering.hh
    KTMultiPeakEventBuilder.hh
    KTMultiPeakTrackProcessing.hh
    KTMultiPeakTrackBuilder.hh
    KTMultiSliceClustering.hh
    KTOverlappingTrackClustering.hh
    KTQuadraticPhaseShift.hh
    KTRPClassifier.hh
    KTSidebandCorrection.hh
    KTSpectrogramCollector.hh
    KTTrackProcessingDoubleCuts.hh
    KTTrackProcessingWeightedSlope.hh
)

set (EVENTANALYSIS_SOURCEFILES
    CutClasses/KTEventFirstTrackNUPCut.cc
    CutClasses/KTEventFirstTrackPowerCut.cc
    CutClasses/KTEventFirstTrackSNRCut.cc
    CutClasses/KTEventTimeCut.cc
    CutClasses/KTEventTimeInAcqCut.cc
    CutClasses/KTLinearFitCut.cc
    CutClasses/KTMultiPeakTrackEventSequenceIDCut.cc
    CutClasses/KTMultiPeakTrackMultiplicityCut.cc
    CutClasses/KTSequentialLineNUPCut.cc
    CutClasses/KTSequentialLinePointDensityCut.cc
    CutClasses/KTSequentialLineSNRCut.cc
    KTCollinearTrackClustering.cc
    CutClasses/KTPowerFitCut.cc
    CutClasses/KTProcessedTrackCut.cc
    CutClasses/KTTrackMainbandCut.cc
    CutClasses/KTTrackMVACut.cc
    CutClasses/KTTrackFreqCut.cc
    CutClasses/KTTrackTimeCut.cc

    KTDataCutter.cc
    KTDBSCANEventClustering.cc
    KTDBSCANTrackClustering.cc
    KTFrequencyCandidateIdentifier.cc
    KTIterativeTrackClustering.cc
    KTMultiPeakEventBuilder.cc
    KTMultiPeakTrackProcessing.cc
    KTMultiPeakTrackBuilder.cc
    KTMultiSliceClustering.cc
    KTOverlappingTrackClustering.cc
    KTRPClassifier.cc
    KTQuadraticPhaseShift.cc
    KTSidebandCorrection.cc
    KTSpectrogramCollector.cc
    KTTrackProcessingDoubleCuts.cc
    KTTrackProcessingWeightedSlope.cc

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
