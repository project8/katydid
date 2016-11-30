# CMakeLists for Katydid/Source/Evaluation
# Author: N. Oblath

set (EVALUATION_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCompareCandidates.hh
)

set (EVALUATION_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCompareCandidates.cc
)

set (KATYDID_LIBS
    KatydidUtility
    KatydidData
)

##################################################

add_library (KatydidEvaluation ${EVALUATION_SOURCEFILES})
target_link_libraries (KatydidEvaluation ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidEvaluation)
pbuilder_install_headers(${EVALUATION_HEADERFILES})
