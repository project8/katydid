# CMakeLists for Katydid/Source/IO
# Author: N. Oblath

set (IO_HEADERFILES
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
    set (IO_DICT_HEADERFILES
        KTDisplayWindow.hh
        Conversions/KTROOTData.hh
    )
    
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        ${IO_DICT_HEADERFILES}
        KTBasicROOTFileWriter.hh
        KTBasicROOTTypeWriterAnalysis.hh
        KTBasicROOTTypeWriterCandidates.hh
        KTBasicROOTTypeWriterEgg.hh
        KTBasicROOTTypeWriterFFT.hh
        KTDataDisplay.hh
        KTDataTypeDisplayAnalysis.hh
        KTDataTypeDisplayEgg.hh
        KTDataTypeDisplayFFT.hh
        KTMultiFileROOTTreeReader.hh
        KTMultiSliceROOTTypeWriterAnalysis.hh
        KTMultiSliceROOTTypeWriterEgg.hh
        KTMultiSliceROOTTypeWriterFFT.hh
        KTMultiSliceROOTWriter.hh
        KTROOTTreeTypeWriterAnalysis.hh
        KTROOTTreeTypeWriterCandidates.hh
        KTROOTTreeTypeWriterEgg.hh
        KTROOTTreeTypeWriterEvaluation.hh
        KTROOTTreeWriter.hh
        Conversions/KT2ROOT.hh
    )

    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        KTBasicROOTTypeWriterAnalysis.cc
        KTBasicROOTTypeWriterCandidates.cc
        KTBasicROOTTypeWriterEgg.cc
        KTBasicROOTTypeWriterFFT.cc
        KTBasicROOTFileWriter.cc
        KTDisplayWindow.cc
        KTDataTypeDisplayAnalysis.cc
        KTDataTypeDisplayEgg.cc
        KTDataTypeDisplayFFT.cc
        KTDataDisplay.cc
        KTMultiFileROOTTreeReader.cc
        KTMultiSliceROOTTypeWriterAnalysis.cc
        KTMultiSliceROOTTypeWriterEgg.cc
        KTMultiSliceROOTTypeWriterFFT.cc
        KTMultiSliceROOTWriter.cc
        KTROOTTreeTypeWriterAnalysis.cc
        KTROOTTreeTypeWriterCandidates.cc
        KTROOTTreeTypeWriterEgg.cc
        KTROOTTreeTypeWriterEvaluation.cc
        KTROOTTreeWriter.cc
        Conversions/KT2ROOT.cc
        Conversions/KTROOTData.cc
    )

    set (IO_LINKDEF_HEADERFILE LinkDef/IOLinkDef.hh)
    set (IO_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict.cxx)
endif (ROOT_FOUND)

if (HDF5_FOUND)
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        KTHDF5TypeWriterFFT.hh
        KTHDF5TypeWriterEgg.hh
        KTHDF5TypeWriterCandidates.hh
        KTHDF5TypeWriterAnalysis.hh
        KTHDF5Writer.hh
    )
    
    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        KTHDF5TypeWriterFFT.cc
        KTHDF5TypeWriterEgg.cc
        KTHDF5TypeWriterCandidates.cc
        KTHDF5TypeWriterAnalysis.cc
        KTHDF5Writer.cc
    )
    
endif (HDF5_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY ("${IO_DICT_HEADERFILES}" "${IO_LINKDEF_HEADERFILE}" "${IO_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
endif (ROOT_FOUND)

add_library (KatydidIO ${IO_SOURCEFILES} ${IO_DICT_OUTFILE})
target_link_libraries (KatydidIO ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${IO_HEADERFILES})
pbuilder_install_libraries (KatydidIO)

