# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
target_link_libraries (SimpleElectronHunt KatydidUtility KatydidEgg KatydidFFT)


install (TARGETS SimpleElectronHunt
         DESTINATION bin)
         