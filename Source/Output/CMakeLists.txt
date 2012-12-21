# CMakeLists for Katydid/Source/Output
# Author: N. Oblath

set (OUTPUT_DICT_HEADER_FILES)

set (OUTPUT_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPublisher.hh
)

if (ROOT_FOUND)
    set (OUTPUT_NODICT_HEADERFILES
        ${OUTPUT_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrumAverager.hh
    )
endif (ROOT_FOUND)

set (OUTPUT_HEADERFILES ${OUTPUT_DICT_HEADERFILES} ${OUTPUT_NODICT_HEADERFILES})

set (OUTPUT_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPublisher.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicAsciiWriter.cc
)

if (ROOT_FOUND)
    set (OUTPUT_SOURCEFILES
        ${OUTPUT_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrumAverager.cc
    )
endif (ROOT_FOUND)


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
target_link_libraries (KatydidOutput ${KATYDID_LIBS} ${LIBS})

katydid_install_headers (${OUTPUT_HEADERFILES})
katydid_install_libraries (KatydidOutput)

