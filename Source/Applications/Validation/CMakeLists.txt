# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (ENABLE_TESTING)

    # Config files
    set (VALIDATION_CONFIGFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/TestParameterStoreConfig.json
    )
    
    katydid_install_config (${VALIDATION_CONFIGFILES})
    

    # Headers for any test classes
    set (VALIDATION_HEADERFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.hh
    )
    
    katydid_install_headers (${VALIDATION_HEADERFILES})
    
    
    # Executables that do NOT require ROOT
    
    #add_executable (ExtractEvents ${CMAKE_CURRENT_SOURCE_DIR}/ExtractEvents.cc)
    #target_link_libraries (ExtractEvents KatydidUtility KatydidEgg ${LIBS})
    
    add_executable (TestApplication ${CMAKE_CURRENT_SOURCE_DIR}/TestApplication.cc
                                    ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestApplication KatydidUtility KatydidCore ${LIBS})

    add_executable (TestAxisProperties ${CMAKE_CURRENT_SOURCE_DIR}/TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KatydidUtility ${LIBS})
    
    add_executable (TestCacheDirectory ${CMAKE_CURRENT_SOURCE_DIR}/TestCacheDirectory.cc)
    target_link_libraries (TestCacheDirectory KatydidUtility KatydidCore ${LIBS})
    
    add_executable (TestComplexFFTW ${CMAKE_CURRENT_SOURCE_DIR}/TestComplexFFTW.cc)
    target_link_libraries (TestComplexFFTW KatydidUtility KatydidCore KatydidEgg KatydidFFT ${LIBS})
    
    add_executable (TestComplexPolar ${CMAKE_CURRENT_SOURCE_DIR}/TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar KatydidUtility ${LIBS})
    
    add_executable (TestCorrelator ${CMAKE_CURRENT_SOURCE_DIR}/TestCorrelator.cc)
    target_link_libraries (TestCorrelator KatydidUtility KatydidEgg KatydidFFT KatydidCore KatydidAnalysis ${LIBS})
        
    add_executable (TestCutIterator ${CMAKE_CURRENT_SOURCE_DIR}/TestCutIterator.cc)
    target_link_libraries (TestCutIterator KatydidUtility ${LIBS})
    
    add_executable (TestCutableArray ${CMAKE_CURRENT_SOURCE_DIR}/TestCutableArray.cc)
    target_link_libraries (TestCutableArray KatydidUtility ${LIBS})
    
    add_executable (TestEggHatching ${CMAKE_CURRENT_SOURCE_DIR}/TestEggHatching.cc)
    target_link_libraries (TestEggHatching KatydidUtility KatydidCore KatydidEgg ${LIBS})
    
    add_executable (TestFrequencySpectrumFFTW ${CMAKE_CURRENT_SOURCE_DIR}/TestFrequencySpectrumFFTW.cc)
    target_link_libraries (TestFrequencySpectrumFFTW KatydidUtility KatydidCore KatydidEgg KatydidFFT ${LIBS})
    
    add_executable (TestLogger ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger.cc)
    target_link_libraries (TestLogger KatydidUtility ${LIBS})
    
    add_executable (TestParameterStore ${CMAKE_CURRENT_SOURCE_DIR}/TestParameterStore.cc
                                       ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestParameterStore KatydidUtility KatydidCore ${LIBS})
    
    add_executable (TestSignalsAndSlots ${CMAKE_CURRENT_SOURCE_DIR}/TestSignalsAndSlots.cc
                                        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KatydidUtility KatydidCore ${LIBS})

    add_executable (TestSimpleFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleFFT.cc)
    target_link_libraries (TestSimpleFFT KatydidUtility KatydidCore KatydidEgg KatydidFFT ${LIBS})

    add_executable (TestSlidingWindowFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSlidingWindowFFT.cc)
    target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidEgg KatydidFFT KatydidCore KatydidAnalysis KatydidOutput ${LIBS})

    add_executable (TestSpectrumDiscriminator ${CMAKE_CURRENT_SOURCE_DIR}/TestSpectrumDiscriminator.cc)
    target_link_libraries (TestSpectrumDiscriminator KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidAnalysis ${LIBS})

    add_executable (TestThroughputProfiler ${CMAKE_CURRENT_SOURCE_DIR}/TestThroughputProfiler.cc)
    target_link_libraries (TestThroughputProfiler KatydidUtility KatydidCore KatydidEgg ${LIBS})

    add_executable (TestVector ${CMAKE_CURRENT_SOURCE_DIR}/TestVector.cc)
    target_link_libraries (TestVector KatydidUtility ${LIBS})

    add_executable (TestWignerVille ${CMAKE_CURRENT_SOURCE_DIR}/TestWignerVille.cc)
    target_link_libraries (TestWignerVille KatydidUtility KatydidEgg KatydidFFT KatydidCore KatydidAnalysis KatydidOutput ${LIBS})

    katydid_install_executables (
                     TestApplication
                     TestAxisProperties
                     TestCacheDirectory
                     TestComplexFFTW
                     TestComplexPolar
                     TestCorrelator
                     TestCutIterator
                     TestCutableArray
                     TestEggHatching
                     TestFrequencySpectrumFFTW
                     TestLogger
                     TestParameterStore
                     TestSignalsAndSlots
                     TestSimpleFFT
                     TestSlidingWindowFFT
                     TestSpectrumDiscriminator
                     TestThroughputProfiler
                     TestVector
                     TestWignerVille
    )
             
    # this command won't run; ExtractEvents needs arguments; I still need to add a data file to the repository to use here.
    #add_test (Test_ExtractEvents ExtractEvents)
    add_test (Test_Application TestApplication)
    add_test (Test_AxisProperties TestAxisProperties)
    add_test (Test_ComplexFFTW TestComplexFFTW)
    add_test (Test_ComplexPolar TestComplexPolar)
    add_test (Test_Correlator TestCorrelator)
    add_test (Test_CutIterator TestCutIterator)
    add_test (Test_CutableArray TestCutableArray)
    add_test (Test_EggHatching TestEggHatching)
    add_test (Test_FrequencySpectrumFFTW TestFrequencySpectrumFFTW)
    add_test (Test_Logger TestLogger)
    add_test (Test_ParameterStore TestParameterStore)
    add_test (Test_SignalsAndSlots TestSignalsAndSlots)
    add_test (Test_SimpleFFT TestSimpleFFT)
    add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
    add_test (Test_SpectrumDiscriminator TestSpectrumDiscriminator)
    add_test (Test_Vector TestVector)
    add_test (Test_WignerVille TestWignerVille)
    
    
    # executables that DO require ROOT
    if (ROOT_FOUND)
        add_executable (TestBasicROOTFileWriter ${CMAKE_CURRENT_SOURCE_DIR}/TestBasicROOTFileWriter.cc)
        target_link_libraries (TestBasicROOTFileWriter KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidOutput ${LIBS})
        
        add_executable (TestGainVariation ${CMAKE_CURRENT_SOURCE_DIR}/TestGainVariation.cc)
        target_link_libraries (TestGainVariation KatydidUtility KatydidCore KatydidEgg KatydidFFT KatydidAnalysis KatydidOutput ${LIBS})
    
        katydid_install_executables ( 
                    TestBasicROOTFileWriter
                    TestGainVariation
        )
    
        add_test (Test_BasicROOTFileWriter TestBasicROOTFileWriter)
    endif (ROOT_FOUND)
        
endif (ENABLE_TESTING) 
