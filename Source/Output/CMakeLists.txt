# CMakeLists for Katydid/Source/Output
# Author: N. Oblath

set (OUTPUT_DICT_HEADER_FILES)

set (OUTPUT_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicAsciiWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOfficialCandidatesWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
)

if (ROOT_FOUND)
    set (OUTPUT_NODICT_HEADERFILES
        ${OUTPUT_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterEgg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterFFT.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.hh
    )
endif (ROOT_FOUND)

set (OUTPUT_HEADERFILES ${OUTPUT_DICT_HEADERFILES} ${OUTPUT_NODICT_HEADERFILES})

set (OUTPUT_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicAsciiWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOfficialCandidatesWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
)

if (ROOT_FOUND)
    set (OUTPUT_SOURCEFILES
        ${OUTPUT_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterEgg.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterFFT.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.cc
    )
endif (ROOT_FOUND)


#if (ROOT_FOUND)
#    set (OUTPUT_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CoreLinkDef.hh)
#    set (OUTPUT_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CoreDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${OUTPUT_DICT_HEADERFILES}" "${OUTPUT_LINKDEF_HEADERFILE}" "${OUTPUT_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidOutput ${OUTPUT_SOURCEFILES})
target_link_libraries (KatydidOutput ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${OUTPUT_HEADERFILES})
pbuilder_install_libraries (KatydidOutput)

