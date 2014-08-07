# CMakeLists for Katydid/Source/Core
# Author: N. Oblath

set (CORE_DICT_HEADERFILES
    KTEventLoop.hh
    KTRootGuiLoop.hh
)

set (CORE_NODICT_HEADERFILES
    KTApplication.hh
    KTCacheDirectory.hh
    KTCommandLineHandler.hh
    KTCommandLineOption.hh
    KTConfigurable.hh
    KTConfigurator.hh
    KTConnection.hh
    KTDataQueueProcessor.hh
    KTDirectory.hh
    KTFilenameParsers.hh
    KTPrimaryProcessor.hh
    KTProcessor.hh
    KTProcessorToolbox.hh
    KTRandom.hh
    KTSignal.hh
    KTSignalWrapper.hh
    KTSlot.hh
    KTSlotWrapper.hh
)

set (CORE_HEADERFILES ${CORE_DICT_HEADERFILES} ${CORE_NODICT_HEADERFILES})

set (CORE_SOURCEFILES
    KTApplication.cc
    KTCacheDirectory.cc
    KTCommandLineHandler.cc
    KTConfigurable.cc
    KTConfigurator.cc
    KTDataQueueProcessor.cc
    KTDirectory.cc
    KTEventLoop.cc
    KTFilenameParsers.cc
    KTPrimaryProcessor.cc
    KTProcessor.cc
    KTProcessorToolbox.cc
    KTRandom.cc
    KTSignal.cc
    KTSignalWrapper.cc
    KTSlotWrapper.cc
)

if (ROOT_FOUND)
    set (CORE_SOURCEFILES
        ${CORE_SOURCEFILES}
        KTRootGuiLoop.cc
    )
endif (ROOT_FOUND)


if (ROOT_FOUND)
    set (CORE_LINKDEF_HEADERFILE LinkDef/CoreLinkDef.hh)
    set (CORE_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CoreDict.cxx)
endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
)

##################################################

if (ROOT_FOUND)
    ROOT_GENERATE_DICTIONARY ("${CORE_DICT_HEADERFILES}" "${CORE_LINKDEF_HEADERFILE}" "${CORE_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
endif (ROOT_FOUND)

add_library (KatydidCore ${CORE_SOURCEFILES} ${CORE_DICT_OUTFILE})
#add_library (KatydidCore ${CORE_SOURCEFILES})
target_link_libraries (KatydidCore ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${CORE_HEADERFILES})
pbuilder_install_libraries (KatydidCore)

