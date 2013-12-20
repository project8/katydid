# CMakeLists for Katydid/Source/IO
# Author: N. Oblath

set (IO_DICT_HEADER_FILES)

set (IO_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicAsciiWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEvaluation.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiFileJSONReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOfficialCandidatesWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTerminalTypeWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTerminalWriter.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWriter.hh
)

if (ROOT_FOUND)
    set (IO_NODICT_HEADERFILES
        ${IO_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiFileROOTTreeReader.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterEgg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterFFT.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTWriter.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterAnalysis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterEvaluation.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeWriter.hh
    )
endif (ROOT_FOUND)

set (IO_HEADERFILES ${IO_DICT_HEADERFILES} ${IO_NODICT_HEADERFILES})

set (IO_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicASCIITypeWriterTS.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicAsciiWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEvaluation.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterCandidates.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiFileJSONReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTOfficialCandidatesWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTerminalTypeWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTerminalWriter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWriter.cc
)

if (ROOT_FOUND)
    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTFileWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiFileROOTTreeReader.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterEgg.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTTypeWriterFFT.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiSliceROOTWriter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterAnalysis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterCandidates.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeTypeWriterEvaluation.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTROOTTreeWriter.cc
    )
endif (ROOT_FOUND)


#if (ROOT_FOUND)
#    set (IO_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CoreLinkDef.hh)
#    set (IO_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CoreDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${IO_DICT_HEADERFILES}" "${IO_LINKDEF_HEADERFILE}" "${IO_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidIO ${IO_SOURCEFILES})
target_link_libraries (KatydidIO ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${IO_HEADERFILES})
pbuilder_install_libraries (KatydidIO)

