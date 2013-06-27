# CMakeLists for Katydid/Source/Input
# Author: N. Oblath

set (INPUT_HEADERFILES
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOneShotJSONReader.hh
)

if (ROOT_FOUND)
    set (INPUT_HEADERFILES
        ${INPUT_HEADERFILES}
    )
endif (ROOT_FOUND)

set (INPUT_SOURCEFILES
    #${CMAKE_CURRENT_SOURCE_DIR}/KTOneShotJSONReader.cc
)

if (ROOT_FOUND)
    set (INPUT_SOURCEFILES
        ${INPUT_SOURCEFILES}
    )
endif (ROOT_FOUND)


set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

add_library (KatydidInput ${INPUT_SOURCEFILES})
target_link_libraries (KatydidInput ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${INPUT_HEADERFILES})
pbuilder_install_libraries (KatydidInput)

