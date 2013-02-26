# CMakeLists for Katydid/Source/Core
# Author: N. Oblath

set (CORE_DICT_HEADER_FILES)

set (CORE_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTApplication.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCacheDirectory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCommandLineHandler.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCommandLineOption.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTConfigurable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTConnection.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDataQueueProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDirectory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTParameterStore.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPrimaryProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTProcessorToolbox.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPStoreNode.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSignal.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTSignalSlotSignature.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSignalWrapper.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlot.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlotWrapper.hh
)

set (CORE_HEADERFILES ${CORE_DICT_HEADERFILES} ${CORE_NODICT_HEADERFILES})

set (CORE_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTApplication.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCacheDirectory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCommandLineHandler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTConfigurable.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDataQueueProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDirectory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTParameterStore.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPrimaryProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTProcessorToolbox.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPStoreNode.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSignal.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTSignalSlotSignature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSignalWrapper.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTSlot.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlotWrapper.cc
)

#if (ROOT_FOUND)
#    set (CORE_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/CoreLinkDef.hh)
#    set (CORE_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/CoreDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${CORE_DICT_HEADERFILES}" "${CORE_LINKDEF_HEADERFILE}" "${CORE_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidCore ${CORE_SOURCEFILES})
target_link_libraries (KatydidCore ${KATYDID_LIBS} ${LIBS})

katydid_install_headers (${CORE_HEADERFILES})
katydid_install_libraries (KatydidCore)

