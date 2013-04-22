# CMakeLists for Katydid/Source/Input
# Author: N. Oblath

set (INPUT_HEADERFILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTReader.hh
)

#if (ROOT_FOUND)
#    set (INPUT_HEADERFILES
#        ${INPUT_HEADERFILES}
#        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileReader.hh
#    )
#endif (ROOT_FOUND)

set (INPUT_SOURCEFILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTReader.cc
)

#if (ROOT_FOUND)
#    set (INPUT_SOURCEFILES
#        ${INPUT_SOURCEFILES}
#        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileReader.cc
#    )
#endif (ROOT_FOUND)


set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

add_library (KatydidInput ${INPUT_SOURCEFILES})
target_link_libraries (KatydidInput ${KATYDID_LIBS} ${LIBS})

pbuilder_install_headers (${INPUT_HEADERFILES})
pbuilder_install_libraries (KatydidInput)

