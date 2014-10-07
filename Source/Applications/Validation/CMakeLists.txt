# CMakeLists for Katydid/Source/Applications/Validation
# Author: N. Oblath

if (Katydid_ENABLE_TESTING)

    # Config files
    set (VALIDATION_CONFIGFILES
        ConfigFiles/TestConfigurator.json
    )
    
    pbuilder_install_config (${VALIDATION_CONFIGFILES})
    

    # Headers for any test classes
    set (VALIDATION_HEADERFILES
        KTTestConfigurable.hh
        KTTestProcessor.hh
    )
    
    pbuilder_install_headers (${VALIDATION_HEADERFILES})
    
    
    # Executables that do NOT require ROOT
    
    add_executable (ObjectSize ObjectSize.cc)
    target_link_libraries (ObjectSize KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestApplication TestApplication.cc
                                    KTTestConfigurable.cc)
    target_link_libraries (TestApplication KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})

    add_executable (TestApplyCut TestApplyCut.cc)
    target_link_libraries (TestApplyCut KatydidUtility KatydidData ${EXTERNAL_LIBRARIES})
    
    add_executable (TestAxisProperties TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCacheDirectory TestCacheDirectory.cc)
    target_link_libraries (TestCacheDirectory KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestComplexPolar TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestConfigurator TestConfigurator.cc
                                       KTTestConfigurable.cc)
    target_link_libraries (TestConfigurator KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestConsensusThresholding TestConsensusThresholding.cc)
    target_link_libraries (TestConsensusThresholding KatydidUtility KatydidIO KatydidData KatydidCore KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCorrelator TestCorrelator.cc)
    target_link_libraries (TestCorrelator KatydidUtility KatydidIO KatydidData KatydidCore KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCut TestCut.cc)
    target_link_libraries (TestCut KatydidUtility KatydidData ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCutIterator TestCutIterator.cc)
    target_link_libraries (TestCutIterator KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCutableArray TestCutableArray.cc)
    target_link_libraries (TestCutableArray KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataAccumulator TestDataAccumulator.cc)
    target_link_libraries (TestDataAccumulator KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataDisplay TestDataDisplay.cc)
    target_link_libraries (TestDataDisplay KatydidUtility KatydidCore KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScan TestDBScan.cc)
    target_link_libraries (TestDBScan KatydidUtility KatydidCore KatydidData KatydidIO KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScanTrackClustering TestDBScanTrackClustering.cc)
    target_link_libraries (TestDBScanTrackClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDistanceClustering TestDistanceClustering.cc)
    target_link_libraries (TestDistanceClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestFrequencySpectrumFFTW TestFrequencySpectrumFFTW.cc)
    target_link_libraries (TestFrequencySpectrumFFTW KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    # temporarily disabled because it's not compatible with the changes made while introducing the extensible data scheme
    #add_executable (TestHoughTransform TestHoughTransform.cc)
    #target_link_libraries (TestHoughTransform KatydidUtility KatydidIO KatydidData KatydidCore KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestJSONWriter TestJSONWriter.cc)
    target_link_libraries (TestJSONWriter KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTree TestKDTree.cc)
    target_link_libraries (TestKDTree KatydidUtility KatydidCore KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTreeData TestKDTreeData.cc)
    target_link_libraries (TestKDTreeData KatydidUtility KatydidCore KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestLogger TestLogger.cc)
    target_link_libraries (TestLogger KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMinMaxBin TestMinMaxBin.cc)
    target_link_libraries (TestMinMaxBin KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiSliceClustering TestMultiSliceClustering.cc)
    target_link_libraries (TestMultiSliceClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiFileJSONReader TestMultiFileJSONReader.cc)
    target_link_libraries (TestMultiFileJSONReader KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestNanoflann TestNanoflann.cc)
    target_link_libraries (TestNanoflann KatydidUtility KatydidCore KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestPrintData TestPrintData.cc)
    target_link_libraries (TestPrintData KatydidUtility KatydidData ${EXTERNAL_LIBRARIES})
    
    add_executable (TestRandom TestRandom.cc)
    target_link_libraries (TestRandom KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSignalsAndSlots TestSignalsAndSlots.cc
                                        KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSmoothing TestSmoothing.cc)
    target_link_libraries (TestSmoothing KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    # disabled because it's written for the old version of KTMultiSliceClustering; see TestMultiSliceClustering
    #add_executable (TestSimpleClustering TestSimpleClustering.cc)
    #target_link_libraries (TestSimpleClustering KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

    #add_executable (TestSlidingWindowFFT TestSlidingWindowFFT.cc)
    #target_link_libraries (TestSlidingWindowFFT KatydidUtility KatydidData KatydidFFT KatydidCore KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestSpectrumDiscriminator TestSpectrumDiscriminator.cc)
    target_link_libraries (TestSpectrumDiscriminator KatydidUtility KatydidCore KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})

    add_executable (TestThroughputProfiler TestThroughputProfiler.cc)
    target_link_libraries (TestThroughputProfiler KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})

    add_executable (TestVector TestVector.cc)
    target_link_libraries (TestVector KatydidUtility ${EXTERNAL_LIBRARIES})

    add_executable (TestWindowFunction TestWindowFunction.cc)
    target_link_libraries (TestWindowFunction KatydidUtility KatydidCore KatydidData KatydidFFT)
    
    #add_executable (TestASCIIFileWriter TestASCIIFileWriter.cc)
    #target_link_libraries (TestASCIIFileWriter KatydidEgg KatydidUtility KatydidCore KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    pbuilder_install_executables (
                     ObjectSize
                     TestApplication
                     TestApplyCut
                     TestAxisProperties
                     TestCacheDirectory
                     TestComplexPolar
                     TestConfigurator
                     TestConsensusThresholding
                     TestCorrelator
                     TestCut
                     TestCutIterator
                     TestCutableArray
                     TestDataAccumulator
                     TestDataDisplay
                     TestDBScan
                     TestDBScanTrackClustering
                     TestDistanceClustering
                     TestFrequencySpectrumFFTW
                     #TestHoughTransform
                     TestJSONWriter
                     TestKDTree
                     TestKDTreeData
                     TestLogger
                     TestMinMaxBin
                     TestMultiSliceClustering
                     TestMultiFileJSONReader
                     TestNanoflann
                     TestPrintData
                     TestRandom
                     TestSignalsAndSlots
                     #TestSimpleClustering
                     #TestSlidingWindowFFT
                     TestSmoothing
                     TestSpectrumDiscriminator
                     TestThroughputProfiler
                     TestVector
                     TestWindowFunction
#                    TestASCIIFileWriter
    )
             
    # this command won't run; ExtractBundles needs arguments; I still need to add a data file to the repository to use here.
    add_test (Test_Application TestApplication)
    add_test (Test_ApplyCut TestApplyCut)
    add_test (Test_AxisProperties TestAxisProperties)
    add_test (Test_CacheDirectory TestCacheDirectory)
    add_test (Test_ComplexPolar TestComplexPolar)
    add_test (Test_Configurator TestConfigurator)
    add_test (Test_Correlator TestCorrelator)
    add_test (Test_Cut TestCut)
    add_test (Test_CutIterator TestCutIterator)
    add_test (Test_CutableArray TestCutableArray)
    add_test (Test_DataAccumulator TestDataAccumulator)
    add_test (Test_DataDisplay TestDataDisplay)
    #add_test (Test_DBScan TestDBScan)
    add_test (Test_DBScanTrackClustering TestDBScanTrackClustering)
    add_test (Test_DistanceClustering TestDistanceClustering)
    add_test (Test_FrequencySpectrumFFTW TestFrequencySpectrumFFTW)
    #add_test (Test_HoughTransform TestHoughTransform)
    add_test (Test_JSONWriter TestJSONWriter)
    add_test (Test_Logger TestLogger)
    add_test (Test_MinMaxBin TestMinMaxBin)
    add_test (Test_MultiSliceClustering TestMultiSliceClustering)
    add_test (Test_MultiFileJSONReader TestMultiFileJSONReader)
    add_test (Test_PrintData TestPrintData)
    add_test (Test_ParameterStore TestParameterStore)
    add_test (Test_Random TestRandom)
    add_test (Test_SignalsAndSlots TestSignalsAndSlots)
    #add_test (Test_SimpleClustering TestSimpleClustering)
    #add_test (Test_SlidingWindowFFT TestSlidingWindowFFT)
    add_test (Test_Smoothing TestSmoothing)
    add_test (Test_SpectrumDiscriminator TestSpectrumDiscriminator)
    add_test (Test_ThroughputProfiler TestThroughputProfiler)
    add_test (Test_Vector TestVector)
    
    
    # executables that DO require FFTW
    if (FFTW_FOUND)
        #include_directories(${CMAKE_CURRENT_SOURCE_DIR})
        #ROOT_GENERATE_DICTIONARY ("guiviewer.h" "guiviewerLinkDef.h" "guiviewerLinkDef.cxx" "${CMAKE_INCLUDE_PATH}")
        #add_library (viewer guiviewer.cxx guiviewerLinkDef.cxx)
        #add_executable (guiviewer guiviewer.cxx guiviewerLinkDef.cxx)
        #target_link_libraries (guiviewer ${EXTERNAL_LIBRARIES})
        #pbuilder_install_libraries (viewer)
        #pbuilder_install_headers (guiviewer.h guiviewerLinkDef.h)
        #pbuilder_install_executables (guiviewer)
    
        add_executable (TestComplexFFTW TestComplexFFTW.cc)
        target_link_libraries (TestComplexFFTW KatydidUtility KatydidCore KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestSimpleFFT TestSimpleFFT.cc)
        target_link_libraries (TestSimpleFFT KatydidUtility KatydidCore KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
    
        add_executable (TestWignerVille TestWignerVille.cc)
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
        add_executable (TestBasicROOTFileWriter TestBasicROOTFileWriter.cc)
        target_link_libraries (TestBasicROOTFileWriter KatydidUtility KatydidCore KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestGainVariation TestGainVariation.cc)
        target_link_libraries (TestGainVariation KatydidUtility KatydidCore KatydidAnalysis KatydidIO ${EXTERNAL_LIBRARIES})
    
        add_executable (TestROOTDictionary TestROOTDictionary.cc)
        target_link_libraries (TestROOTDictionary KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestBasicROOTFileWriter
                    TestGainVariation
                    TestROOTDictionary
        )
    
        add_test (Test_BasicROOTFileWriter TestBasicROOTFileWriter)
        add_test (Test_GainVariation TestGainVariation)
        add_test (Test_ROOTDictionary TestROOTDictionary)        
    endif (ROOT_FOUND)
    
    # executables that DO require Monarch
    if (Katydid_USE_MONARCH)
        add_executable (TestEggHatching TestEggHatching.cc)
        target_link_libraries (TestEggHatching KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestEggHatching
        )

        add_test (Test_EggHatching TestEggHatching)
    endif (Katydid_USE_MONARCH)
        
endif (Katydid_ENABLE_TESTING) 
