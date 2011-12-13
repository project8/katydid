add_executable (ExtractEvents ${PROJECT_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
target_link_libraries (ExtractEvents KatydidEgg)

add_executable (SimpleElectronHunt ${PROJECT_CURRENT_SOURCE_DIR}/SimpleElectronHunt.cc)
target_link_libraries (SimpleElectronHunt KatydidEgg KatydidFFT)

install (TARGETS ExtractEvents
                 SimpleElectronHunt
         DESTINATION bin)
         