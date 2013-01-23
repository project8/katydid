# CMakeLists for Katydid/Source/Candidates
# Author: N. Oblath

set (CANDIDATES_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
)

if (ROOT_FOUND)
    set (CANDIDATES_HEADERFILES
        ${CANDIDATES_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.hh
    )
endif (ROOT_FOUND)

set (CANDIDATES_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.cc
)

if (ROOT_FOUND)
    set (CANDIDATES_SOURCEFILES
        ${CANDIDATES_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.cc
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

add_library (KatydidCandidates ${CANDIDATES_SOURCEFILES})
target_link_libraries (KatydidCandidates ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidCandidates)
katydid_install_headers(${CANDIDATES_HEADERFILES})
