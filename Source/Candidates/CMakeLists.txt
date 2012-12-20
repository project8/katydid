# CMakeLists for Katydid/Source/Candidates
# Author: N. Oblath

set (CANDIDATES_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.hh
)

set (CANDIDATES_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidate.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencyCandidateIdentifier.cc
)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidEgg
    KatydidFFT
    KatydidAnalysis
)

##################################################

add_library (KatydidCandidates ${CANDIDATES_SOURCEFILES})
target_link_libraries (KatydidCandidates ${KATYDID_LIBS} ${LIBS})

katydid_install_libraries(KatydidCandidates)
katydid_install_headers(${CANDIDATES_HEADERFILES})
