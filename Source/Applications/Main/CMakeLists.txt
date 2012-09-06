# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

# Config files
set (VALIDATION_CONFIGFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ChannelCorrelatorConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ExtractPowerSpectraConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/SimpleElectronHunt2Config.json
)

install (FILES ${VALIDATION_CONFIGFILES}  DESTINATION ${CONFIG_INSTALL_DIR})
    

# Executables

add_executable (Truncate ${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cc)
target_link_libraries (Truncate KatydidUtility)

install (TARGETS 
            Truncate
         DESTINATION ${BIN_INSTALL_DIR})


if (ROOT_FOUND)
    add_executable (ChannelCorrelator ${CMAKE_CURRENT_SOURCE_DIR}/ChannelCorrelator.cc)
    target_link_libraries (ChannelCorrelator KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidOutput KatydidAnalysis)

    add_executable (ExtractPowerSpectra ${CMAKE_CURRENT_SOURCE_DIR}/ExtractPowerSpectra.cc)
    target_link_libraries (ExtractPowerSpectra KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidOutput)
    
    add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
    target_link_libraries (SimpleElectronHunt KatydidUtility KatydidCore KatydidEgg KatydidFFT)
    
    add_executable (SimpleElectronHunt2 ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt2.cc)
    target_link_libraries (SimpleElectronHunt2 KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidAnalysis)
    
    install (TARGETS
                ChannelCorrelator
                ExtractPowerSpectra
                SimpleElectronHunt
                SimpleElectronHunt2
             DESTINATION ${BIN_INSTALL_DIR})
endif (ROOT_FOUND)
