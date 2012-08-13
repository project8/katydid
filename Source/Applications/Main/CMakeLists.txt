# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
target_link_libraries (SimpleElectronHunt KatydidUtility KatydidEgg KatydidFFT)

add_executable (SimpleElectronHunt2 ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt2.cc)
target_link_libraries (SimpleElectronHunt2 KatydidUtility KatydidEgg KatydidFFT KatydidAnalysis)

add_executable (ExtractPowerSpectra ${CMAKE_CURRENT_SOURCE_DIR}/ExtractPowerSpectra.cc)
target_link_libraries (ExtractPowerSpectra KatydidUtility KatydidEgg KatydidFFT)

add_executable (Truncate ${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cc)
target_link_libraries (Truncate KatydidUtility)

install (TARGETS SimpleElectronHunt
                 SimpleElectronHunt2
                 ExtractPowerSpectra
                 Truncate
         DESTINATION ${BIN_INSTALL_DIR})
         