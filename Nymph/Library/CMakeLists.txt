# CMakeLists for Nymph/Library
# Author: N. Oblath

set (UTIL_DIR Utility)
set (DATA_DIR Data)
set (PROC_DIR Processor)
set (IO_DIR IO)
set (APPL_DIR Application)

set (NYMPH_HEADERFILES
    ${UTIL_DIR}/KTCacheDirectory.hh
    ${UTIL_DIR}/KTConcurrentQueue.hh
    ${UTIL_DIR}/KTConfigurable.hh
    ${UTIL_DIR}/KTDestroyer.hh
    ${UTIL_DIR}/KTDirectory.hh
    ${UTIL_DIR}/KTEventLoop.hh
    ${UTIL_DIR}/KTException.hh
    ${UTIL_DIR}/KTExtensibleStruct.hh
    ${UTIL_DIR}/KTExtensibleStructFactory.hh
    ${UTIL_DIR}/KTFactory.hh
    ${UTIL_DIR}/KTLogger.hh
    ${UTIL_DIR}/KTMemberVariable.hh
    ${UTIL_DIR}/KTNOFactory.hh
    ${UTIL_DIR}/KTParam.hh
    ${UTIL_DIR}/KTParamInputJSON.hh
    ${UTIL_DIR}/KTSingleton.hh
    ${UTIL_DIR}/KTTIFactory.hh
    ${UTIL_DIR}/KTTime.hh
    ${DATA_DIR}/KTApplyCut.hh
    ${DATA_DIR}/KTCut.hh
    ${DATA_DIR}/KTCutResult.hh
    ${DATA_DIR}/KTCutStatus.hh
    ${DATA_DIR}/KTData.hh
    ${PROC_DIR}/KTConnection.hh
    ${PROC_DIR}/KTPrimaryProcessor.hh
    ${PROC_DIR}/KTProcessor.hh
    ${PROC_DIR}/KTSignal.hh
    ${PROC_DIR}/KTSignalWrapper.hh
    ${PROC_DIR}/KTSlot.hh
    ${PROC_DIR}/KTSlotWrapper.hh
    ${IO_DIR}/KTReader.hh
    ${IO_DIR}/KTWriter.hh
    ${APPL_DIR}/KTApplication.hh
    ${APPL_DIR}/KTCommandLineHandler.hh
    ${APPL_DIR}/KTCommandLineOption.hh
    ${APPL_DIR}/KTConfigurator.hh
    ${APPL_DIR}/KTDataQueueProcessor.hh
    ${APPL_DIR}/KTFilenameParsers.hh
    ${APPL_DIR}/KTPrintDataStructure.hh
    ${APPL_DIR}/KTProcessorToolbox.hh
    ${APPL_DIR}/KTThroughputProfiler.hh
)

set (NYMPH_SOURCEFILES
    ${UTIL_DIR}/KTCacheDirectory.cc
    ${UTIL_DIR}/KTConfigurable.cc
    ${UTIL_DIR}/KTDirectory.cc
    ${UTIL_DIR}/KTEventLoop.cc
    ${UTIL_DIR}/KTException.cc
    ${UTIL_DIR}/KTLogger.cc
    ${UTIL_DIR}/KTParam.cc
    ${UTIL_DIR}/KTParamInputJSON.cc
    ${UTIL_DIR}/KTTime.cc
    ${DATA_DIR}/KTApplyCut.cc
    ${DATA_DIR}/KTCut.cc
    ${DATA_DIR}/KTCutStatus.cc
    ${DATA_DIR}/KTData.cc
    ${PROC_DIR}/KTPrimaryProcessor.cc
    ${PROC_DIR}/KTProcessor.cc
    ${PROC_DIR}/KTSignal.cc
    ${PROC_DIR}/KTSignalWrapper.cc
    ${PROC_DIR}/KTSlotWrapper.cc
    ${IO_DIR}/KTReader.cc
    ${IO_DIR}/KTWriter.cc
    ${APPL_DIR}/KTApplication.cc
    ${APPL_DIR}/KTCommandLineHandler.cc
    ${APPL_DIR}/KTConfigurator.cc
    ${APPL_DIR}/KTDataQueueProcessor.cc
    ${APPL_DIR}/KTFilenameParsers.cc
    ${APPL_DIR}/KTPrintDataStructure.cc
    ${APPL_DIR}/KTProcessorToolbox.cc
    ${APPL_DIR}/KTThroughputProfiler.cc
)

##################################################

add_library (Nymph ${NYMPH_SOURCEFILES})
target_link_libraries (Nymph ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${NYMPH_HEADERFILES})
pbuilder_install_libraries (Nymph)

