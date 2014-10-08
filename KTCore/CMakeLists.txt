# CMakeLists for Katydid/KTCore
# Author: N. Oblath

set (KTCORE_HEADERFILES
    Utility/KTCacheDirectory.hh
    Utility/KTConcurrentQueue.hh
    Utility/KTConfigurable.hh
    Utility/KTDestroyer.hh
    Utility/KTDirectory.hh
    Utility/KTException.hh
    Utility/KTExtensibleStruct.hh
    Utility/KTExtensibleStructFactory.hh
    Utility/KTFactory.hh
    Utility/KTLogger.hh
    Utility/KTMemberVariable.hh
    Utility/KTNOFactory.hh
    Utility/KTParam.hh
    Utility/KTParamInputJSON.hh
    Utility/KTSingleton.hh
    Utility/KTTIFactory.hh
    Utility/KTTime.hh
    Data/KTApplyCut.hh
    Data/KTCut.hh
    Data/KTCutResult.hh
    Data/KTCutStatus.hh
    Data/KTData.hh
    Processor/KTConnection.hh
    Processor/KTPrimaryProcessor.hh
    Processor/KTProcessor.hh
    Processor/KTSignal.hh
    Processor/KTSignalWrapper.hh
    Processor/KTSlot.hh
    Processor/KTSlotWrapper.hh
    Application/KTApplication.hh
    Application/KTCommandLineHandler.hh
    Application/KTCommandLineOption.hh
    Application/KTConfigurator.hh
    Application/KTDataQueueProcessor.hh
    Application/KTFilenameParsers.hh
    Application/KTPrintDataStructure.hh
    Application/KTProcessorToolbox.hh
    Application/KTThroughputProfiler.hh
)

set (KTCORE_SOURCEFILES
    Utility/KTCacheDirectory.cc
    Utility/KTConfigurable.cc
    Utility/KTDirectory.cc
    Utility/KTException.cc
    Utility/KTLogger.cc
    Utility/KTParam.cc
    Utility/KTParamInputJSON.cc
    Utility/KTTime.cc
    Data/KTApplyCut.cc
    Data/KTCut.cc
    Data/KTCutStatus.cc
    Data/KTData.cc
    Processor/KTPrimaryProcessor.cc
    Processor/KTProcessor.cc
    Processor/KTSignal.cc
    Processor/KTSignalWrapper.cc
    Processor/KTSlotWrapper.cc
    Application/KTApplication.cc
    Application/KTCommandLineHandler.cc
    Application/KTConfigurator.cc
    Application/KTDataQueueProcessor.cc
    Application/KTFilenameParsers.cc
    Application/KTPrintDataStructure.cc
    Application/KTProcessorToolbox.cc
    Application/KTThroughputProfiler.cc
)

##################################################

add_library (KTCore ${KTCORE_SOURCEFILES})
target_link_libraries (KTCore ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${KTCORE_HEADERFILES})
pbuilder_install_libraries (KTCore)

