# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
target_link_libraries (SimpleElectronHunt KatydidUtility KatydidEgg KatydidFFT)

add_executable (ExtractPowerSpectra ${CMAKE_CURRENT_SOURCE_DIR}/ExtractPowerSpectra.cc)
target_link_libraries (ExtractPowerSpectra KatydidUtility KatydidEgg KatydidFFT)

install (TARGETS SimpleElectronHunt
                 ExtractPowerSpectra
         DESTINATION bin)
         