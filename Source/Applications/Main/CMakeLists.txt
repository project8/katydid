# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

# Config files
set (VALIDATION_CONFIGFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/AveragePowerSpectrumConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ChannelCorrelatorConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ExtractPowerSpectraConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/FSPeakAnalysisConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSComplexConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSConfig.json
    #${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSMultithreadedConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidWVConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/SimpleElectronHunt2Config.json
)

katydid_install_config (${VALIDATION_CONFIGFILES})
    

# Executables

add_executable (ConfigCheck ${CMAKE_CURRENT_SOURCE_DIR}/ConfigCheck.cc)
target_link_libraries (ConfigCheck KatydidUtility KatydidCore ${LIBS})

add_executable (Katydid ${CMAKE_CURRENT_SOURCE_DIR}/Katydid.cc)
target_link_libraries (Katydid KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${LIBS})

add_executable (Truncate ${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cc)
target_link_libraries (Truncate KatydidUtility ${LIBS})

katydid_install_executables ( 
    ConfigCheck
    Katydid
    Truncate
)


if (ROOT_FOUND)
    add_executable (ChannelCorrelator ${CMAKE_CURRENT_SOURCE_DIR}/ChannelCorrelator.cc)
    target_link_libraries (ChannelCorrelator KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT KatydidAnalysis ${LIBS})

    add_executable (ExtractPowerSpectra ${CMAKE_CURRENT_SOURCE_DIR}/ExtractPowerSpectra.cc)
    target_link_libraries (ExtractPowerSpectra KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT ${LIBS})
    
    #add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
    #target_link_libraries (SimpleElectronHunt KatydidUtility KatydidCore KatydidEgg KatydidFFT ${LIBS})
    
    add_executable (SimpleElectronHunt2 ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt2.cc)
    target_link_libraries (SimpleElectronHunt2 KatydidUtility KatydidCore KatydidOutput KatydidEgg KatydidFFT KatydidAnalysis ${LIBS})
    
    katydid_install_executables ( 
                ChannelCorrelator
                ExtractPowerSpectra
                #SimpleElectronHunt
                SimpleElectronHunt2
     )
endif (ROOT_FOUND)
