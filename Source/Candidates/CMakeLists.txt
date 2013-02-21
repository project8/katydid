# CMakeLists for Katydid/Source/Candidates
# Author: N. Oblath

#set (CANDIDATES_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CandidatesLinkDef.hh)
#set (CANDIDATES_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CandidatesDict.cxx)

set (CANDIDATES_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceClustering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWaterfallCandidateData.hh
)

if (ROOT_FOUND)
    set (CANDIDATES_DICT_HEADERFILES
        #${CMAKE_CURRENT_SOURCE_DIR}/TFrequencyCandidateData.hh
    )

    set (CANDIDATES_HEADERFILES
        ${CANDIDATES_HEADERFILES}
        ${CANDIDATES_DICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.hh
    )
endif (ROOT_FOUND)

set (CANDIDATES_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceClustering.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWaterfallCandidateData.cc
)

if (ROOT_FOUND)
    set (CANDIDATES_SOURCEFILES
        ${CANDIDATES_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.cc
        #${CMAKE_CURRENT_SOURCE_DIR}/TFrequencyCandidateData.cc
        #${CANDIDATES_DICT_OUTFILE}
    )
endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidOutput
    KatydidEgg
    KatydidFFT
    KatydidAnalysis
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${CANDIDATES_DICT_HEADERFILES}" "${CANDIDATES_LINKDEF_HEADERFILE}" "${CANDIDATES_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidCandidates ${CANDIDATES_SOURCEFILES})
target_link_libraries (KatydidCandidates ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidCandidates)
katydid_install_headers(${CANDIDATES_HEADERFILES})
