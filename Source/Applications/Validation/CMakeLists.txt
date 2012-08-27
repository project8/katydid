# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (ENABLE_TESTING)

    # Config files
    set (VALIDATION_CONFIGFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/TestParameterStoreConfig.json
    )
    
    install (FILES ${VALIDATION_CONFIGFILES}  DESTINATION ${CONFIG_INSTALL_DIR})
    

    # Headers for any test classes
    set (VALIDATION_HEADERFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.hh
    )
    
    install (FILES ${VALIDATION_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
    
    
    # Executables that do NOT require ROOT
    
    #add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
    #target_link_libraries (ExtractEvents KatydidUtility KatydidEgg ${LIBS})
    
    add_executable (TestApplication ${CMAKE_CURRENT_SOURCE_DIR}/TestApplication.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestApplication KatydidUtility KatydidCore ${LIBS})

    add_executable (TestAxisProperties ${CMAKE_CURRENT_SOURCE_DIR}/TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KatydidUtility ${LIBS})
    
    add_executable (TestBasicROOTFilePublisher ${CMAKE_CURRENT_SOURCE_DIR}/TestBasicROOTFilePublisher.cc)
    target_link_libraries (TestBasicROOTFilePublisher KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidOutput ${LIBS})
    
    add_executable (TestComplexPolar ${CMAKE_CURRENT_SOURCE_DIR}/TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar KatydidUtility ${LIBS})
    
    add_executable (TestCutIterator ${CMAKE_CURRENT_SOURCE_DIR}/TestCutIterator.cc)
    target_link_libraries (TestCutIterator KatydidUtility ${LIBS})
    
    add_executable (TestCutableArray ${CMAKE_CURRENT_SOURCE_DIR}/TestCutableArray.cc)
    target_link_libraries (TestCutableArray KatydidUtility ${LIBS})
    
    add_executable (TestEggHatching ${CMAKE_CURRENT_SOURCE_DIR}/TestEggHatching.cc)
    target_link_libraries (TestEggHatching KatydidUtility KatydidCore KatydidEgg ${LIBS})
    
    add_executable (TestLogger ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger.cc)
    target_link_libraries (TestLogger KatydidUtility ${LIBS})
    
    add_executable (TestParameterStore ${CMAKE_CURRENT_SOURCE_DIR}/TestParameterStore.cc
                                       ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestParameterStore KatydidUtility KatydidCore ${LIBS})
    
    add_executable (TestSignalsAndSlots ${CMAKE_CURRENT_SOURCE_DIR}/TestSignalsAndSlots.cc
                                        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KatydidUtility KatydidCore ${LIBS})

    add_executable (TestVector ${CMAKE_CURRENT_SOURCE_DIR}/TestVector.cc)
    target_link_libraries (TestVector KatydidUtility ${LIBS})
    
    install (TARGETS #ExtractEvents
                     TestApplication
                     TestAxisProperties
                     TestBasicROOTFilePublisher
                     TestComplexPolar
                     TestCutIterator
                     TestCutableArray
                     TestEggHatching
                     TestLogger
                     TestParameterStore
                     TestSignalsAndSlots
                     TestVector
             DESTINATION ${BIN_INSTALL_DIR})
             
    # this command won't run; ExtractEvents needs arguments; I still need to add a data file to the repository to use here.
    #add_test (Test_ExtractEvents ExtractEvents)
    add_test (Test_Application TestApplication)
    add_test (Test_AxisProperties TestAxisProperties)
    add_test (Test_BasicROOTFilePublisher TestBasicROOTFilePublisher)
    add_test (Test_ComplexPolar TestComplexPolar)
    add_test (Test_CutIterator TestCutIterator)
    add_test (Test_CutableArray TestCutableArray)
    add_test (Test_EggHatching TestEggHatching)
    add_test (Test_Logger TestLogger)
    add_test (Test_ParameterStore TestParameterStore)
    add_test (Test_SignalsAndSlots TestSignalsAndSlots)
    add_test (Test_Vector TestVector)
    
    
    # executables that DO require ROOT
    if (ROOT_FOUND)
        add_executable (TestSimpleFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleFFT.cc)
        target_link_libraries (TestSimpleFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})
    
        add_executable (TestSlidingWindowFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSlidingWindowFFT.cc)
        target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidEgg KatydidFFT ${LIBS})
    
        
        install (TARGETS TestSimpleFFT
                         TestSlidingWindowFFT
                 DESTINATION ${BIN_INSTALL_DIR})
    
        add_test (Test_SimpleFFT TestSimpleFFT)
        add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
    endif (ROOT_FOUND)
        
endif (ENABLE_TESTING) 
