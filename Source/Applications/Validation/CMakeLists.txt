# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (ENABLE_TESTING)

    add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
    target_link_libraries (ExtractEvents KatydidUtility KatydidEgg ${LIBS})
    
    add_executable (TestSimpleFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleFFT.cc)
    target_link_libraries (TestSimpleFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})

    add_executable (TestSlidingWindowFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSlidingWindowFFT.cc)
    target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})

    install (TARGETS ExtractEvents
                     TestSimpleFFT
                     TestSlidingWindowFFT
             DESTINATION bin)
             
    # this command won't run; ExtractEvents needs arguments; I still need to add a data file to the repository to use here.
    add_test (Test_ExtractEvents ExtractEvents)
    add_test (Test_SimpleFFT TestSimpleFFT)
    add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
        
endif (ENABLE_TESTING) 
