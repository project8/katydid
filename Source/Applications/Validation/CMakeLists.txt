# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (Katydid_ENABLE_TESTING)

    # Config files
    set (VALIDATION_CONFIGFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/TestParameterStoreConfig.json
    )
    
    pbuilder_install_config (${VALIDATION_CONFIGFILES})
    

    # Headers for any test classes
    set (VALIDATION_HEADERFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.hh
    )
    
    pbuilder_install_headers (${VALIDATION_HEADERFILES})
    
    
    # Executables that do NOT require ROOT
    
    add_executable (TestApplication ${CMAKE_CURRENT_SOURCE_DIR}/TestApplication.cc
                                    ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestApplication KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})

    add_executable (TestAxisProperties ${CMAKE_CURRENT_SOURCE_DIR}/TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCacheDirectory ${CMAKE_CURRENT_SOURCE_DIR}/TestCacheDirectory.cc)
    target_link_libraries (TestCacheDirectory KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestComplexPolar ${CMAKE_CURRENT_SOURCE_DIR}/TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCorrelator ${CMAKE_CURRENT_SOURCE_DIR}/TestCorrelator.cc)
    target_link_libraries (TestCorrelator KatydidUtility KatydidIO KatydidData KatydidCore KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCutIterator ${CMAKE_CURRENT_SOURCE_DIR}/TestCutIterator.cc)
    target_link_libraries (TestCutIterator KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCutableArray ${CMAKE_CURRENT_SOURCE_DIR}/TestCutableArray.cc)
    target_link_libraries (TestCutableArray KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDistanceClustering ${CMAKE_CURRENT_SOURCE_DIR}/TestDistanceClustering.cc)
    target_link_libraries (TestDistanceClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestFrequencySpectrumFFTW ${CMAKE_CURRENT_SOURCE_DIR}/TestFrequencySpectrumFFTW.cc)
    target_link_libraries (TestFrequencySpectrumFFTW KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    # temporarily disabled because it's not compatible with the changes made while introducing the extensible data scheme
    #add_executable (TestHoughTransform ${CMAKE_CURRENT_SOURCE_DIR}/TestHoughTransform.cc)
    #target_link_libraries (TestHoughTransform KatydidUtility KatydidIO KatydidData KatydidCore KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestJSONWriter ${CMAKE_CURRENT_SOURCE_DIR}/TestJSONWriter.cc)
    target_link_libraries (TestJSONWriter KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    add_executable (TestLogger ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger.cc)
    target_link_libraries (TestLogger KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiSliceClustering ${CMAKE_CURRENT_SOURCE_DIR}/TestMultiSliceClustering.cc)
    target_link_libraries (TestMultiSliceClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiFileJSONReader ${CMAKE_CURRENT_SOURCE_DIR}/TestMultiFileJSONReader.cc)
    target_link_libraries (TestMultiFileJSONReader KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestParameterStore ${CMAKE_CURRENT_SOURCE_DIR}/TestParameterStore.cc
                                       ${CMAKE_CURRENT_SOURCE_DIR}/KTTestConfigurable.cc)
    target_link_libraries (TestParameterStore KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestRandom ${CMAKE_CURRENT_SOURCE_DIR}/TestRandom.cc)
    target_link_libraries (TestRandom KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSignalsAndSlots ${CMAKE_CURRENT_SOURCE_DIR}/TestSignalsAndSlots.cc
                                        ${CMAKE_CURRENT_SOURCE_DIR}/KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})

    # disabled because it's written for the old version of KTMultiSliceClustering; see TestMultiSliceClustering
    #add_executable (TestSimpleClustering ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleClustering.cc)
    #target_link_libraries (TestSimpleClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

    #add_executable (TestSlidingWindowFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSlidingWindowFFT.cc)
    #target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidData KatydidFFT KatydidCore KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestSpectrumDiscriminator ${CMAKE_CURRENT_SOURCE_DIR}/TestSpectrumDiscriminator.cc)
    target_link_libraries (TestSpectrumDiscriminator KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})

    add_executable (TestThroughputProfiler ${CMAKE_CURRENT_SOURCE_DIR}/TestThroughputProfiler.cc)
    target_link_libraries (TestThroughputProfiler KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})

    add_executable (TestVector ${CMAKE_CURRENT_SOURCE_DIR}/TestVector.cc)
    target_link_libraries (TestVector KatydidUtility ${EXTERNAL_LIBRARIES})

    add_executable (TestWindowFunction ${CMAKE_CURRENT_SOURCE_DIR}/TestWindowFunction.cc)
    target_link_libraries (TestWindowFunction KatydidUtility KatydidCore KatydidData KatydidFFT)
    
    #add_executable (TestASCIIFileWriter ${CMAKE_CURRENT_SOURCE_DIR}/TestASCIIFileWriter.cc)
    #target_link_libraries (TestASCIIFileWriter KatydidEgg KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    pbuilder_install_executables (
                     TestApplication
                     TestAxisProperties
                     TestCacheDirectory
                     TestComplexPolar
                     TestCorrelator
                     TestCutIterator
                     TestCutableArray
                     TestDistanceClustering
                     TestFrequencySpectrumFFTW
                     #TestHoughTransform
                     TestJSONWriter
                     TestLogger
                     TestMultiSliceClustering
                     TestMultiFileJSONReader
                     TestParameterStore
                     TestRandom
                     TestSignalsAndSlots
                     #TestSimpleClustering
                     #TestSlidingWindowFFT
                     TestSpectrumDiscriminator
                     TestThroughputProfiler
                     TestVector
                     TestWindowFunction
#                    TestASCIIFileWriter
    )
             
    # this command won't run; ExtractBundles needs arguments; I still need to add a data file to the repository to use here.
    add_test (Test_Application TestApplication)
    add_test (Test_AxisProperties TestAxisProperties)
    add_test (Test_CacheDirectory TestCacheDirectory)
    add_test (Test_ComplexPolar TestComplexPolar)
    add_test (Test_Correlator TestCorrelator)
    add_test (Test_CutIterator TestCutIterator)
    add_test (Test_CutableArray TestCutableArray)
    add_test (Test_DistanceClustering TestDistanceClustering)
    add_test (Test_FrequencySpectrumFFTW TestFrequencySpectrumFFTW)
    #add_test (Test_HoughTransform TestHoughTransform)
    add_test (Test_JSONWriter TestJSONWriter)
    add_test (Test_Logger TestLogger)
    add_test (Test_MultiSliceClustering TestMultiSliceClustering)
    add_test (Test_MultiFileJSONReader TestMultiFileJSONReader)
    add_test (Test_ParameterStore TestParameterStore)
    add_test (Test_Random TestRandom)
    add_test (Test_SignalsAndSlots TestSignalsAndSlots)
    #add_test (Test_SimpleClustering TestSimpleClustering)
    #add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
    add_test (Test_SpectrumDiscriminator TestSpectrumDiscriminator)
    add_test (Test_ThroughputProfiler TestThroughputProfiler)
    add_test (Test_Vector TestVector)
    
    
    # executables that DO require FFTW
    if (FFTW_FOUND)
        add_executable (TestComplexFFTW ${CMAKE_CURRENT_SOURCE_DIR}/TestComplexFFTW.cc)
        target_link_libraries (TestComplexFFTW KatydidUtility KatydidCore KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestSimpleFFT ${CMAKE_CURRENT_SOURCE_DIR}/TestSimpleFFT.cc)
        target_link_libraries (TestSimpleFFT KatydidUtility KatydidCore KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
    
        add_executable (TestWignerVille ${CMAKE_CURRENT_SOURCE_DIR}/TestWignerVille.cc)
        target_link_libraries (TestWignerVille KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables (
                         TestComplexFFTW
                         TestSimpleFFT
                         TestWignerVille
        )
        
        add_test (Test_ComplexFFTW TestComplexFFTW)
        add_test (Test_SimpleFFT TestSimpleFFT)
        add_test (Test_WignerVille TestWignerVille)
    endif (FFTW_FOUND)
    
    
    # executables that DO require ROOT
    if (ROOT_FOUND)
        add_executable (TestBasicROOTFileWriter ${CMAKE_CURRENT_SOURCE_DIR}/TestBasicROOTFileWriter.cc)
        target_link_libraries (TestBasicROOTFileWriter KatydidUtility KatydidCore KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestGainVariation ${CMAKE_CURRENT_SOURCE_DIR}/TestGainVariation.cc)
        target_link_libraries (TestGainVariation KatydidUtility KatydidCore KatydidAnalysis KatydidIO ${EXTERNAL_LIBRARIES})
    
        pbuilder_install_executables ( 
                    TestBasicROOTFileWriter
                    TestGainVariation
        )
    
        add_test (Test_BasicROOTFileWriter TestBasicROOTFileWriter)
    endif (ROOT_FOUND)
    
    # executables that DO require Monarch
    if (Katydid_USE_MONARCH)
        add_executable (TestEggHatching ${CMAKE_CURRENT_SOURCE_DIR}/TestEggHatching.cc)
        target_link_libraries (TestEggHatching KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestEggHatching
        )

        add_test (Test_EggHatching TestEggHatching)
    endif (Katydid_USE_MONARCH)
        
endif (Katydid_ENABLE_TESTING) 
