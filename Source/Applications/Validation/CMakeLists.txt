# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (ENABLE_TESTING)

    add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
    target_link_libraries (ExtractEvents KatydidUtility KatydidEgg ${LIBS})

    install (TARGETS ExtractEvents
             DESTINATION bin)
             
    # this command won't run; ExtractEvents needs arguments; I still need to add a data file to the repository to use here.
    add_test (Test_ExtractEvents ExtractEvents)
        
endif (ENABLE_TESTING) 