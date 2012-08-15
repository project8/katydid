# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (ENABLE_TESTING)

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.hh DESTINATION ${INCLUDE_INSTALL_DIR})

    add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
    target_link_libraries (ExtractEvents KatydidUtility KatydidEgg ${LIBS})
    
    add_executable (TestAxisProperties ${CMAKE_CURRENT_SOURCE_DIR}/TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KatydidUtility ${LIBS})
    
    add_executable (TestCutIterator ${CMAKE_CURRENT_SOURCE_DIR}/TestCutIterator.cc)
    target_link_libraries (TestCutIterator KatydidUtility ${LIBS})
    
    add_executable (TestCutableArray ${CMAKE_CURRENT_SOURCE_DIR}/TestCutableArray.cc)
    target_link_libraries (TestCutableArray KatydidUtility ${LIBS})
    
    add_executable (TestLogger ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger.cc)
    target_link_libraries (TestLogger KatydidUtility ${LIBS})
    
    add_executable (TestSettings ${CMAKE_CURRENT_SOURCE_DIR}/TestSettings.cc)
    target_link_libraries (TestSettings KatydidUtility ${LIBS})
    
    add_executable (TestSignalsAndSlots ${CMAKE_CURRENT_SOURCE_DIR}/TestSignalsAndSlots.cc
                                        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KatydidUtility ${LIBS})

    add_executable (TestSimpleFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleFFT.cc)
    target_link_libraries (TestSimpleFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})

    add_executable (TestSlidingWindowFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSlidingWindowFFT.cc)
    target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})

    add_executable (TestVector ${CMAKE_CURRENT_SOURCE_DIR}/TestVector.cc)
    target_link_libraries (TestVector KatydidUtility ${LIBS})
    
    install (TARGETS ExtractEvents
                     TestAxisProperties
                     TestCutIterator
                     TestCutableArray
                     TestLogger
                     TestSettings
                     TestSignalsAndSlots
                     TestSimpleFFT
                     TestSlidingWindowFFT
                     TestVector
             DESTINATION ${BIN_INSTALL_DIR})
             
    # this command won't run; ExtractEvents needs arguments; I still need to add a data file to the repository to use here.
    add_test (Test_ExtractEvents ExtractEvents)
    add_test (Test_AxisProperties TestAxisProperties)
    add_test (Test_CutIterator TestCutIterator)
    add_test (Test_CutableArray TestCutableArray)
    add_test (Test_Logger TestLogger)
    add_test (Test_Settings TestSettings)
    add_test (Test_SignalsAndSlots TestSignalsAndSlots)
    add_test (Test_SimpleFFT TestSimpleFFT)
    add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
    add_test (Test_Vector TestVector)
        
endif (ENABLE_TESTING) 
