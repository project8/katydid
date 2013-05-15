# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

# Config files
set (VALIDATION_CONFIGFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/AveragePowerSpectrumConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ChannelCorrelatorConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/EvaluateCandidates.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ExtractPowerSpectraConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/FSPeakAnalysisConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSComplexConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSConfig.json
    #${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSMultithreadedConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidWVConfig.json
)

pbuilder_install_config (${VALIDATION_CONFIGFILES})
    

# Executables

add_executable (EvaluateCandidates ${CMAKE_CURRENT_SOURCE_DIR}/Katydid.cc)
target_link_libraries (EvaluateCandidates KatydidUtility KatydidCore KatydidData KatydidInput KatydidEvaluation ${LIBS})

add_executable (ConfigCheck ${CMAKE_CURRENT_SOURCE_DIR}/ConfigCheck.cc)
target_link_libraries (ConfigCheck KatydidUtility KatydidCore ${LIBS})

add_executable (EggScanner ${CMAKE_CURRENT_SOURCE_DIR}/EggScanner.cc)
target_link_libraries (EggScanner KatydidUtility KatydidCore KatydidData KatydidOutput KatydidEgg ${LIBS})

add_executable (Katydid ${CMAKE_CURRENT_SOURCE_DIR}/Katydid.cc)
target_link_libraries (Katydid KatydidUtility KatydidCore KatydidData KatydidOutput KatydidSimulation KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${LIBS})

add_executable (Truncate ${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cc)
target_link_libraries (Truncate KatydidUtility ${LIBS})

pbuilder_install_executables ( 
    ConfigCheck
    EggScanner
    EvaluateCandidates
    Katydid
    Truncate
)


if (ROOT_FOUND)
    add_executable (ChannelCorrelator ${CMAKE_CURRENT_SOURCE_DIR}/ChannelCorrelator.cc)
    target_link_libraries (ChannelCorrelator KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT KatydidAnalysis ${LIBS})

    add_executable (ExtractPowerSpectra ${CMAKE_CURRENT_SOURCE_DIR}/ExtractPowerSpectra.cc)
    target_link_libraries (ExtractPowerSpectra KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT ${LIBS})
    
    pbuilder_install_executables ( 
                ChannelCorrelator
                ExtractPowerSpectra
     )
endif (ROOT_FOUND)
