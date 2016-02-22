# CMakeLists for Katydid/Source/IO
# Author: N. Oblath

set (IO_HEADERFILES
    KTBasicASCIITypeWriterTS.hh
    KTBasicAsciiWriter.hh
    KTJSONTypeWriterTime.hh
    KTJSONTypeWriterEvaluation.hh
    KTJSONTypeWriterEventAnalysis.hh
    KTJSONWriter.hh
    KTMultiFileJSONReader.hh
    KTOfficialCandidatesWriter.hh
    KTTerminalTypeWriterTime.hh
    KTTerminalWriter.hh
)

set (IO_SOURCEFILES
    KTBasicASCIITypeWriterTS.cc
    KTBasicAsciiWriter.cc
    KTJSONTypeWriterTime.cc
    KTJSONTypeWriterEvaluation.cc
    KTJSONTypeWriterEventAnalysis.hh
    KTJSONWriter.cc
    KTMultiFileJSONReader.cc
    KTOfficialCandidatesWriter.cc
    KTTerminalTypeWriterTime.cc
    KTTerminalWriter.cc
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
        KTBasicROOTTypeWriterSpectrumAnalysis.hh
        KTBasicROOTTypeWriterEventAnalysis.hh
        KTBasicROOTTypeWriterTime.hh
        KTBasicROOTTypeWriterTransform.hh
        KTDataDisplay.hh
        KTDataTypeDisplaySpectrumAnalysis.hh
        KTDataTypeDisplayTime.hh
        KTDataTypeDisplayTransform.hh
        KTMultiFileROOTTreeReader.hh
        KTMultiSliceROOTTypeWriterSpectrumAnalysis.hh
        KTMultiSliceROOTTypeWriterTime.hh
        KTMultiSliceROOTTypeWriterTransform.hh
        KTMultiSliceROOTWriter.hh
        KTROOTSpectrogramTypeWriterTransform.hh
        KTROOTSpectrogramWriter.hh
        KTROOTTreeTypeWriterSpectrumAnalysis.hh
        KTROOTTreeTypeWriterEventAnalysis.hh
        KTROOTTreeTypeWriterTime.hh
        KTROOTTreeTypeWriterEvaluation.hh
        KTROOTTreeWriter.hh
        Conversions/KT2ROOT.hh
    )

    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        KTBasicROOTTypeWriterSpectrumAnalysis.cc
        KTBasicROOTTypeWriterEventAnalysis.cc
        KTBasicROOTTypeWriterTime.cc
        KTBasicROOTTypeWriterTransform.cc
        KTBasicROOTFileWriter.cc
        KTDisplayWindow.cc
        KTDataTypeDisplaySpectrumAnalysis.cc
        KTDataTypeDisplayTime.cc
        KTDataTypeDisplayTransform.cc
        KTDataDisplay.cc
        KTMultiFileROOTTreeReader.cc
        KTMultiSliceROOTTypeWriterSpectrumAnalysis.cc
        KTMultiSliceROOTTypeWriterTime.cc
        KTMultiSliceROOTTypeWriterTransform.cc
        KTMultiSliceROOTWriter.cc
        KTROOTSpectrogramTypeWriterTransform.cc
        KTROOTSpectrogramWriter.cc
        KTROOTTreeTypeWriterSpectrumAnalysis.cc
        KTROOTTreeTypeWriterEventAnalysis.cc
        KTROOTTreeTypeWriterTime.cc
        KTROOTTreeTypeWriterEvaluation.cc
        KTROOTTreeWriter.cc
        Conversions/KT2ROOT.cc
        Conversions/KTROOTData.cc
    )

    set (IO_LINKDEF_HEADERFILE LinkDef/IOLinkDef.hh)
    set (IO_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict.cxx)
    set (IO_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict_rdict.pcm)
endif (ROOT_FOUND)

if (HDF5_FOUND)
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        KTHDF5TypeWriterTransform.hh
        KTHDF5TypeWriterTime.hh
        KTHDF5TypeWriterEventAnalysis.hh
        KTHDF5TypeWriterSpectrumAnalysis.hh
        KTHDF5Writer.hh
    )
    
    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        KTHDF5TypeWriterTransform.cc
        KTHDF5TypeWriterTime.cc
        KTHDF5TypeWriterEventAnalysis.cc
        KTHDF5TypeWriterSpectrumAnalysis.cc
        KTHDF5Writer.cc
    )
    
endif (HDF5_FOUND)

set (KATYDID_LIBS
    Nymph
    KatydidUtility
    KatydidData
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY (IODict ${IO_DICT_HEADERFILES} LINKDEF ${IO_LINKDEF_HEADERFILE})
    if (Katydid_USE_CPP11)
        pbuilder_install_files (${LIB_INSTALL_DIR} ${IO_DICT_PCMFILE})
    endif (Katydid_USE_CPP11)
endif (ROOT_FOUND)

add_library (KatydidIO ${IO_SOURCEFILES} ${IO_DICT_OUTFILE})
target_link_libraries (KatydidIO ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${IO_HEADERFILES})
pbuilder_install_libraries (KatydidIO)

