# CMakeLists for Katydid/Source/IO
# Author: N. Oblath

set (IO_NODICT_HEADERFILES
    KTBasicASCIITypeWriterTS.hh
    KTBasicAsciiWriter.hh
    KTJSONTypeWriterEgg.hh
    KTJSONTypeWriterEvaluation.hh
    KTJSONTypeWriterCandidates.hh
    KTJSONWriter.hh
    KTMultiFileJSONReader.hh
    KTOfficialCandidatesWriter.hh
    KTTerminalTypeWriterEgg.hh
    KTTerminalWriter.hh
    KTReader.hh
    KTWriter.hh
)

if (ROOT_FOUND)
    if (NOT Katydid_SINGLETHREADED)
        set (IO_DICT_HEADERFILES
            ${IO_DICT_HEADERFILES}
            KTDisplayWindow.hh
            #${CMAKE_CURRENT_SOURCE_DIR}/../Core/KTRootGuiLoop.hh
        )
        
        set (IO_NODICT_HEADERFILES
            ${IO_NODICT_HEADERFILES}
            #KTDataDisplay.hh
            #KTDataTypeDisplayAnalysis.hh
            #KTDataTypeDisplayEgg.hh
            #KTDataTypeDisplayFFT.hh
        )
    endif (NOT Katydid_SINGLETHREADED)
    
    set (IO_NODICT_HEADERFILES
        ${IO_NODICT_HEADERFILES}
        KTBasicROOTFileWriter.hh
        KTBasicROOTTypeWriterAnalysis.hh
        KTBasicROOTTypeWriterEgg.hh
        KTBasicROOTTypeWriterFFT.hh
        KTMultiFileROOTTreeReader.hh
        KTMultiSliceROOTTypeWriterAnalysis.hh
        KTMultiSliceROOTTypeWriterEgg.hh
        KTMultiSliceROOTTypeWriterFFT.hh
        KTMultiSliceROOTWriter.hh
        KTROOTTreeTypeWriterAnalysis.hh
        KTROOTTreeTypeWriterCandidates.hh
        KTROOTTreeTypeWriterEvaluation.hh
        KTROOTTreeWriter.hh
    )
endif (ROOT_FOUND)

set (IO_HEADERFILES ${IO_DICT_HEADERFILES} ${IO_NODICT_HEADERFILES})

set (IO_SOURCEFILES
    KTBasicASCIITypeWriterTS.cc
    KTBasicAsciiWriter.cc
    KTJSONTypeWriterEgg.cc
    KTJSONTypeWriterEvaluation.cc
    KTJSONTypeWriterCandidates.hh
    KTJSONWriter.cc
    KTMultiFileJSONReader.cc
    KTOfficialCandidatesWriter.cc
    KTTerminalTypeWriterEgg.cc
    KTTerminalWriter.cc
    KTReader.cc
    KTWriter.cc
)

if (ROOT_FOUND)
    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        KTBasicROOTTypeWriterAnalysis.cc
        KTBasicROOTTypeWriterEgg.cc
        KTBasicROOTTypeWriterFFT.cc
        KTBasicROOTFileWriter.cc
        KTMultiFileROOTTreeReader.cc
        KTMultiSliceROOTTypeWriterAnalysis.cc
        KTMultiSliceROOTTypeWriterEgg.cc
        KTMultiSliceROOTTypeWriterFFT.cc
        KTMultiSliceROOTWriter.cc
        KTROOTTreeTypeWriterAnalysis.cc
        KTROOTTreeTypeWriterCandidates.cc
        KTROOTTreeTypeWriterEvaluation.cc
        KTROOTTreeWriter.cc
    )
    
    if (NOT Katydid_SINGLETHREADED)
        set (IO_SOURCEFILES
            ${IO_SOURCEFILES}
            KTDisplayWindow.cc
            #${CMAKE_CURRENT_SOURCE_DIR}/../Core/KTRootGuiLoop.cc
            #KTDataTypeDisplayAnalysis.cc
            #KTDataTypeDisplayEgg.cc
            #KTDataTypeDisplayFFT.cc
            #KTDataDisplay.cc
        )
    endif (NOT Katydid_SINGLETHREADED)
endif (ROOT_FOUND)


if (NOT Katydid_SINGLETHREADED)
    if (ROOT_FOUND)
        set (IO_LINKDEF_HEADERFILE LinkDef/IOLinkDef.hh)
        set (IO_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict.cxx)
    endif (ROOT_FOUND)
endif (NOT Katydid_SINGLETHREADED)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

if (NOT Katydid_SINGLETHREADED)
    if (ROOT_FOUND)
        ROOT_GENERATE_DICTIONARY ("${IO_DICT_HEADERFILES}" "${IO_LINKDEF_HEADERFILE}" "${IO_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
    endif (ROOT_FOUND)
endif (NOT Katydid_SINGLETHREADED)

add_library (KatydidIO ${IO_SOURCEFILES} ${IO_DICT_OUTFILE})
target_link_libraries (KatydidIO ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${IO_HEADERFILES})
pbuilder_install_libraries (KatydidIO)

