# CMakeLists for Katydid/Source/Executables/Validation
# Author: N. Oblath

if (Katydid_ENABLE_TESTING)

    # Executables that do NOT require ROOT
    
    add_executable (ObjectSize ObjectSize.cc)
    target_link_libraries (ObjectSize KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestAxisProperties TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestComplexPolar TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestConsensusThresholding TestConsensusThresholding.cc)
    target_link_libraries (TestConsensusThresholding KTCore KatydidUtility KatydidIO KatydidData KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCorrelator TestCorrelator.cc)
    target_link_libraries (TestCorrelator KTCore KatydidUtility KatydidIO KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCutIterator TestCutIterator.cc)
    target_link_libraries (TestCutIterator KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCutableArray TestCutableArray.cc)
    target_link_libraries (TestCutableArray KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataAccumulator TestDataAccumulator.cc)
    target_link_libraries (TestDataAccumulator KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataDisplay TestDataDisplay.cc)
    target_link_libraries (TestDataDisplay KTCore KatydidUtility KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScan TestDBScan.cc)
    target_link_libraries (TestDBScan KTCore KatydidUtility KatydidData KatydidIO KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScanTrackClustering TestDBScanTrackClustering.cc)
    target_link_libraries (TestDBScanTrackClustering KTCore KatydidUtility KatydidData KatydidIO KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDistanceClustering TestDistanceClustering.cc)
    target_link_libraries (TestDistanceClustering KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestFrequencySpectrumFFTW TestFrequencySpectrumFFTW.cc)
    target_link_libraries (TestFrequencySpectrumFFTW KTCore KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    # temporarily disabled because it's not compatible with the changes made while introducing the extensible data scheme
    #add_executable (TestHoughTransform TestHoughTransform.cc)
    #target_link_libraries (TestHoughTransform KTCore KatydidUtility KatydidIO KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestJSONWriter TestJSONWriter.cc)
    target_link_libraries (TestJSONWriter KTCore KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTree TestKDTree.cc)
    target_link_libraries (TestKDTree KTCore KatydidUtility KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTreeData TestKDTreeData.cc)
    target_link_libraries (TestKDTreeData KTCore KatydidUtility KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMinMaxBin TestMinMaxBin.cc)
    target_link_libraries (TestMinMaxBin KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiSliceClustering TestMultiSliceClustering.cc)
    target_link_libraries (TestMultiSliceClustering KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiFileJSONReader TestMultiFileJSONReader.cc)
    target_link_libraries (TestMultiFileJSONReader KTCore KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestNanoflann TestNanoflann.cc)
    target_link_libraries (TestNanoflann KTCore KatydidUtility KatydidData KatydidAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestRandom TestRandom.cc)
    target_link_libraries (TestRandom KTCore KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSmoothing TestSmoothing.cc)
    target_link_libraries (TestSmoothing KTCore KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    # disabled because it's written for the old version of KTMultiSliceClustering; see TestMultiSliceClustering
    #add_executable (TestSimpleClustering TestSimpleClustering.cc)
    #target_link_libraries (TestSimpleClustering KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

    #add_executable (TestSlidingWindowFFT TestSlidingWindowFFT.cc)
    #target_link_libraries (TestSlidingWindowFFT KTCore KatydidUtility KatydidData KatydidFFT KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestSpectrumDiscriminator TestSpectrumDiscriminator.cc)
    target_link_libraries (TestSpectrumDiscriminator KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})

    add_executable (TestVector TestVector.cc)
    target_link_libraries (TestVector KTCore KatydidUtility ${EXTERNAL_LIBRARIES})

    add_executable (TestWindowFunction TestWindowFunction.cc)
    target_link_libraries (TestWindowFunction KTCore KatydidUtility KatydidData KatydidFFT)
    
    #add_executable (TestASCIIFileWriter TestASCIIFileWriter.cc)
    #target_link_libraries (TestASCIIFileWriter KTCore KatydidTime KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    pbuilder_install_executables (
                     ObjectSize
                     TestAxisProperties
                     TestComplexPolar
                     TestConsensusThresholding
                     TestCorrelator
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
                     TestMinMaxBin
                     TestMultiSliceClustering
                     TestMultiFileJSONReader
                     TestNanoflann
                     TestRandom
                     #TestSimpleClustering
                     #TestSlidingWindowFFT
                     TestSmoothing
                     TestSpectrumDiscriminator
                     TestVector
                     TestWindowFunction
#                    TestASCIIFileWriter
    )
             
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
        target_link_libraries (TestComplexFFTW KTCore KatydidUtility KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestSimpleFFT TestSimpleFFT.cc)
        target_link_libraries (TestSimpleFFT KTCore KatydidUtility KatydidData KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
    
        add_executable (TestWignerVille TestWignerVille.cc)
        target_link_libraries (TestWignerVille KTCore KatydidUtility KatydidData KatydidIO KatydidAnalysis ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables (
                         TestComplexFFTW
                         TestSimpleFFT
                         TestWignerVille
        )
    endif (FFTW_FOUND)
    
    
    # executables that DO require ROOT
    if (ROOT_FOUND)
        add_executable (TestBasicROOTFileWriter TestBasicROOTFileWriter.cc)
        target_link_libraries (TestBasicROOTFileWriter KTCore KatydidUtility KatydidIO KatydidFFT ${EXTERNAL_LIBRARIES})
        
        add_executable (TestGainVariation TestGainVariation.cc)
        target_link_libraries (TestGainVariation KTCore KatydidUtility KatydidAnalysis KatydidIO ${EXTERNAL_LIBRARIES})
    
        add_executable (TestROOTDictionary TestROOTDictionary.cc)
        target_link_libraries (TestROOTDictionary KTCore KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestBasicROOTFileWriter
                    TestGainVariation
                    TestROOTDictionary
        )
    endif (ROOT_FOUND)
    
    # executables that DO require Monarch
    if (Katydid_USE_MONARCH)
        add_executable (TestEggHatching TestEggHatching.cc)
        target_link_libraries (TestEggHatching KTCore KatydidUtility KatydidData KatydidIO KatydidTime ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestEggHatching
        )
    endif (Katydid_USE_MONARCH)
        
endif (Katydid_ENABLE_TESTING) 
