# CMakeLists for Katydid/Source/Output
# Author: N. Oblath

set (OUTPUT_DICT_HEADER_FILES)

set (OUTPUT_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFilePublisher.hh
)

set (OUTPUT_HEADERFILES ${OUTPUT_DICT_HEADERFILES} ${OUTPUT_NODICT_HEADERFILES})

set (OUTPUT_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFilePublisher.cc
)

#if (ROOT_FOUND)
#    set (OUTPUT_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CoreLinkDef.hh)
#    set (OUTPUT_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CoreDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidEgg
    KatydidFFT
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${OUTPUT_DICT_HEADERFILES}" "${OUTPUT_LINKDEF_HEADERFILE}" "${OUTPUT_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidOutput ${OUTPUT_SOURCEFILES})
set_target_properties(KatydidOutput PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
target_link_libraries (KatydidOutput ${KATYDID_LIBS} ${LIBS})

install (FILES ${OUTPUT_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
install (TARGETS KatydidOutput  DESTINATION ${LIB_INSTALL_DIR})

