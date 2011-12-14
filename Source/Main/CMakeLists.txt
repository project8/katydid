add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
target_link_libraries (ExtractEvents KatydidEgg ${LIBS})

add_executable (SimpleElectronHunt ${CMAKE_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
target_link_libraries (SimpleElectronHunt KatydidEgg KatydidFFT)

install (TARGETS ExtractEvents
                 SimpleElectronHunt
         DESTINATION bin)
         