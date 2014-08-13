# CMakeLists for Katydid/Source/Candidates
# Author: N. Oblath

#set (CANDIDATES_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CandidatesLinkDef.hh)
#set (CANDIDATES_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CandidatesDict.cxx)

set (CANDIDATES_HEADERFILES
    KTCandidateCleaner.hh
    KTCandidateTrackFinder.hh
    KTDBScan.hh
    KTDistanceMatrix.hh
    #KTDBScanEventClustering.hh
    KTDBScanTrackClustering.hh
    KTFrequencyCandidateIdentifier.hh
    KTMultiSliceClustering.hh
    KTTrackProcessing.hh
)

set (CANDIDATES_SOURCEFILES
    KTCandidateCleaner.cc
    KTCandidateTrackFinder.cc
    #KTDBScanEventClustering.cc
    KTDBScanTrackClustering.cc
    KTFrequencyCandidateIdentifier.cc
    KTMultiSliceClustering.cc
    KTTrackProcessing.cc
)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
    KatydidIO
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${CANDIDATES_DICT_HEADERFILES}" "${CANDIDATES_LINKDEF_HEADERFILE}" "${CANDIDATES_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidCandidates ${CANDIDATES_SOURCEFILES})
target_link_libraries (KatydidCandidates ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidCandidates)
pbuilder_install_headers(${CANDIDATES_HEADERFILES})
