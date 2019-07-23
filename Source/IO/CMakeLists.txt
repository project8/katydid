# CMakeLists for Katydid/Source/IO
# Author: N. Oblath

set (IO_HEADERFILES
    BasicAsciiWriter/KTBasicASCIITypeWriterTS.hh
    BasicAsciiWriter/KTBasicAsciiWriter.hh
    JSONWriter/KTJSONTypeWriterTime.hh
    #JSONWriter/KTJSONTypeWriterEvaluation.hh
    JSONWriter/KTJSONTypeWriterEventAnalysis.hh
    JSONWriter/KTJSONWriter.hh
    OfficialCandidatesWriter/KTOfficialCandidatesWriter.hh
    TerminalWriter/KTTerminalTypeWriterEventAnalysis.hh
    TerminalWriter/KTTerminalTypeWriterTime.hh
    TerminalWriter/KTTerminalWriter.hh
    KTDPTReader.hh
    #KTMultiFileJSONReader.hh
)

set (IO_SOURCEFILES
    BasicAsciiWriter/KTBasicASCIITypeWriterTS.cc
    BasicAsciiWriter/KTBasicAsciiWriter.cc
    JSONWriter/KTJSONTypeWriterTime.cc
    #JSONWriter/KTJSONTypeWriterEvaluation.cc
    JSONWriter/KTJSONTypeWriterEventAnalysis.cc
    JSONWriter/KTJSONWriter.cc
    OfficialCandidatesWriter/KTOfficialCandidatesWriter.cc
    TerminalWriter/KTTerminalTypeWriterEventAnalysis.cc
    TerminalWriter/KTTerminalTypeWriterTime.cc
    TerminalWriter/KTTerminalWriter.cc
    KTDPTReader.cc
    #KTMultiFileJSONReader.cc
)

if (ROOT_FOUND)
    set (IO_DICT_HEADERFILES
        DataDisplay/KTDisplayWindow.hh
        Conversions/KTROOTData.hh
    )
    
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        ${IO_DICT_HEADERFILES}
        Conversions/KT2ROOT.hh
        BasicROOTFileWriter/KTBasicROOTFileWriter.hh
        BasicROOTFileWriter/KTBasicROOTTypeWriterSpectrumAnalysis.hh
        BasicROOTFileWriter/KTBasicROOTTypeWriterEventAnalysis.hh
        BasicROOTFileWriter/KTBasicROOTTypeWriterTime.hh
        BasicROOTFileWriter/KTBasicROOTTypeWriterTransform.hh
        DataDisplay/KTDataDisplay.hh
        DataDisplay/KTDataTypeDisplayEventAnalysis.hh
        DataDisplay/KTDataTypeDisplaySpectrumAnalysis.hh
        DataDisplay/KTDataTypeDisplayTime.hh
        DataDisplay/KTDataTypeDisplayTransform.hh
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterSpectrumAnalysis.hh
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterTime.hh
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterTransform.hh
        MultiSliceROOTWriter/KTMultiSliceROOTWriter.hh
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterEventAnalysis.hh
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterSpectrumAnalysis.hh
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterTransform.hh
        ROOTSpectrogramWriter/KTROOTSpectrogramWriter.hh
        ROOTTreeWriter/KTROOTTreeTypeWriterSpectrumAnalysis.hh
        ROOTTreeWriter/KTROOTTreeTypeWriterEventAnalysis.hh
        ROOTTreeWriter/KTROOTTreeTypeWriterTime.hh
        #ROOTTreeWriter/KTROOTTreeTypeWriterEvaluation.hh
        ROOTTreeWriter/KTROOTTreeWriter.hh
        KTROOTWriterFileManager.hh
        KTMultiFileROOTTreeReader.hh
    )

    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        Conversions/KT2ROOT.cc
        Conversions/KTROOTData.cc
        BasicROOTFileWriter/KTBasicROOTTypeWriterSpectrumAnalysis.cc
        BasicROOTFileWriter/KTBasicROOTTypeWriterEventAnalysis.cc
        BasicROOTFileWriter/KTBasicROOTTypeWriterTime.cc
        BasicROOTFileWriter/KTBasicROOTTypeWriterTransform.cc
        BasicROOTFileWriter/KTBasicROOTFileWriter.cc
        DataDisplay/KTDataTypeDisplayEventAnalysis.cc
        DataDisplay/KTDataTypeDisplaySpectrumAnalysis.cc
        DataDisplay/KTDataTypeDisplayTime.cc
        DataDisplay/KTDataTypeDisplayTransform.cc
        DataDisplay/KTDataDisplay.cc
        DataDisplay/KTDisplayWindow.cc
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterSpectrumAnalysis.cc
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterTime.cc
        MultiSliceROOTWriter/KTMultiSliceROOTTypeWriterTransform.cc
        MultiSliceROOTWriter/KTMultiSliceROOTWriter.cc
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterEventAnalysis.cc
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterSpectrumAnalysis.cc
        ROOTSpectrogramWriter/KTROOTSpectrogramTypeWriterTransform.cc
        ROOTSpectrogramWriter/KTROOTSpectrogramWriter.cc
        ROOTTreeWriter/KTROOTTreeTypeWriterSpectrumAnalysis.cc
        ROOTTreeWriter/KTROOTTreeTypeWriterEventAnalysis.cc
        ROOTTreeWriter/KTROOTTreeTypeWriterTime.cc
        #ROOTTreeWriter/KTROOTTreeTypeWriterEvaluation.cc
        ROOTTreeWriter/KTROOTTreeWriter.cc
        KTROOTWriterFileManager.cc
        KTMultiFileROOTTreeReader.cc
    )

    set (IO_LINKDEF_HEADERFILE LinkDef/IOLinkDef.hh)
    set (IO_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict.cxx)
    set (IO_DICT_PCMFILE ${CMAKE_CURRENT_BINARY_DIR}/IODict_rdict.pcm)
endif (ROOT_FOUND)

if (HDF5_FOUND)
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        HDF5Writer/KTHDF5TypeWriterTransform.hh
        HDF5Writer/KTHDF5TypeWriterTime.hh
        HDF5Writer/KTHDF5TypeWriterEventAnalysis.hh
        HDF5Writer/KTHDF5TypeWriterSpectrumAnalysis.hh
        HDF5Writer/KTHDF5Writer.hh
    )
    
    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        HDF5Writer/KTHDF5TypeWriterTransform.cc
        HDF5Writer/KTHDF5TypeWriterTime.cc
        HDF5Writer/KTHDF5TypeWriterEventAnalysis.cc
        HDF5Writer/KTHDF5TypeWriterSpectrumAnalysis.cc
        HDF5Writer/KTHDF5Writer.cc
    )
endif (HDF5_FOUND)

if (MAGICK++_FOUND)
    set (IO_HEADERFILES
        ${IO_HEADERFILES}
        ImageWriter/KTImageTypeWriterTransform.hh
        ImageWriter/KTImageWriter.hh
    )

    set (IO_SOURCEFILES
        ${IO_SOURCEFILES}
        ImageWriter/KTImageTypeWriterTransform.cc
        ImageWriter/KTImageWriter.cc
    )
endif (MAGICK++_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidData
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY (IODict ${IO_DICT_HEADERFILES} LINKDEF ${IO_LINKDEF_HEADERFILE} OPTIONS -inlineInputHeader)
    pbuilder_install_files (${LIB_INSTALL_DIR} ${IO_DICT_PCMFILE})
endif (ROOT_FOUND)

add_library (KatydidIO ${IO_SOURCEFILES} ${IO_DICT_OUTFILE})
target_link_libraries (KatydidIO ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${IO_HEADERFILES})
pbuilder_install_libraries (KatydidIO)

