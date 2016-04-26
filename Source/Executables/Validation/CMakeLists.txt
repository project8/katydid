# CMakeLists for Katydid/Source/Executables/Validation
# Author: N. Oblath

if (Katydid_ENABLE_TESTING)

    # Executables that do NOT require ROOT
    
    add_executable (ObjectSize ObjectSize.cc)
    target_link_libraries (ObjectSize Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (Test2DDiscrim Test2DDiscrim.cc)
    target_link_libraries (Test2DDiscrim Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})

    add_executable (TestAxisProperties TestAxisProperties.cc)
    target_link_libraries (TestAxisProperties Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestComplexPolar TestComplexPolar.cc)
    target_link_libraries (TestComplexPolar Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestConsensusThresholding TestConsensusThresholding.cc)
    target_link_libraries (TestConsensusThresholding Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCorrelator TestCorrelator.cc)
    target_link_libraries (TestCorrelator Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestCutIterator TestCutIterator.cc)
    target_link_libraries (TestCutIterator Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCutableArray TestCutableArray.cc)
    target_link_libraries (TestCutableArray Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataAccumulator TestDataAccumulator.cc)
    target_link_libraries (TestDataAccumulator Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDataDisplay TestDataDisplay.cc)
    target_link_libraries (TestDataDisplay Nymph KatydidUtility KatydidData KatydidIO KatydidTransform ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScan TestDBScan.cc)
    target_link_libraries (TestDBScan Nymph KatydidUtility KatydidData KatydidIO KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDBScanTrackClustering TestDBScanTrackClustering.cc)
    target_link_libraries (TestDBScanTrackClustering Nymph KatydidUtility KatydidData KatydidIO KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestDistanceClustering TestDistanceClustering.cc)
    target_link_libraries (TestDistanceClustering Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
        
    #add_executable (TestFrequencySpectrumFFTW TestFrequencySpectrumFFTW.cc)
    #target_link_libraries (TestFrequencySpectrumFFTW Nymph KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    # temporarily disabled because it's not compatible with the changes made while introducing the extensible data scheme
    #add_executable (TestHoughTransform TestHoughTransform.cc)
    #target_link_libraries (TestHoughTransform Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
        
    add_executable (TestJSONWriter TestJSONWriter.cc)
    target_link_libraries (TestJSONWriter Nymph KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTree TestKDTree.cc)
    target_link_libraries (TestKDTree Nymph KatydidUtility KatydidData KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestKDTreeData TestKDTreeData.cc)
    target_link_libraries (TestKDTreeData Nymph KatydidUtility KatydidData KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
 
    add_executable (TestLinearDensityProbe TestLinearDensityProbe.cc)
    target_link_libraries (TestLinearDensityProbe Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMinMaxBin TestMinMaxBin.cc)
    target_link_libraries (TestMinMaxBin Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiSliceClustering TestMultiSliceClustering.cc)
    target_link_libraries (TestMultiSliceClustering Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestMultiFileJSONReader TestMultiFileJSONReader.cc)
    target_link_libraries (TestMultiFileJSONReader Nymph KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestNanoflann TestNanoflann.cc)
    target_link_libraries (TestNanoflann Nymph KatydidUtility KatydidData KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestRandom TestRandom.cc)
    target_link_libraries (TestRandom Nymph KatydidUtility ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSmoothing TestSmoothing.cc)
    target_link_libraries (TestSmoothing Nymph KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    # disabled because it's written for the old version of KTMultiSliceClustering; see TestMultiSliceClustering
    #add_executable (TestSimpleClustering TestSimpleClustering.cc)
    #target_link_libraries (TestSimpleClustering Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})

    #add_executable (TestSlidingWindowFFT TestSlidingWindowFFT.cc)
    #target_link_libraries (TestSlidingWindowFFT Nymph KatydidUtility KatydidData KatydidTransform KatydidIO ${EXTERNAL_LIBRARIES})

    add_executable (TestSpectrogramCollector TestSpectrogramCollector.cc)
    target_link_libraries (TestSpectrogramCollector Nymph KatydidUtility KatydidIO KatydidData KatydidSpectrumAnalysis KatydidEventAnalysis ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSpectrumDiscriminator TestSpectrumDiscriminator.cc)
    target_link_libraries (TestSpectrumDiscriminator Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})

    add_executable (TestVector TestVector.cc)
    target_link_libraries (TestVector Nymph KatydidUtility ${EXTERNAL_LIBRARIES})

    add_executable (TestWindowFunction TestWindowFunction.cc)
    target_link_libraries (TestWindowFunction Nymph KatydidUtility KatydidData KatydidTransform)
    
    #add_executable (TestASCIIFileWriter TestASCIIFileWriter.cc)
    #target_link_libraries (TestASCIIFileWriter Nymph KatydidTime KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})

    pbuilder_install_executables (
                     ObjectSize
                     Test2DDiscrim
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
                     #TestFrequencySpectrumFFTW
                     #TestHoughTransform
                     TestJSONWriter
                     TestKDTree
                     TestKDTreeData
                     TestLinearDensityProbe
                     TestMinMaxBin
                     TestMultiSliceClustering
                     TestMultiFileJSONReader
                     TestNanoflann
                     TestRandom
                     #TestSimpleClustering
                     #TestSlidingWindowFFT
                     TestSmoothing
                     TestSpectrogramCollector
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
    
        add_executable (TestForwardFFTW TestForwardFFTW.cc)
        target_link_libraries (TestForwardFFTW Nymph KatydidUtility KatydidData KatydidIO KatydidTransform ${EXTERNAL_LIBRARIES})
        
        #add_executable (TestSimpleFFT TestSimpleFFT.cc)
        #target_link_libraries (TestSimpleFFT Nymph KatydidUtility KatydidData KatydidIO KatydidTransform ${EXTERNAL_LIBRARIES})
    
        add_executable (TestWignerVille TestWignerVille.cc)
        target_link_libraries (TestWignerVille Nymph KatydidUtility KatydidData KatydidIO KatydidSpectrumAnalysis ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables (
                         TestForwardFFTW
                         #TestSimpleFFT
                         TestWignerVille
        )
    endif (FFTW_FOUND)
    
    
    # executables that DO require ROOT
    if (ROOT_FOUND)
        add_executable (TestBasicROOTFileWriter TestBasicROOTFileWriter.cc)
        target_link_libraries (TestBasicROOTFileWriter Nymph KatydidUtility KatydidIO KatydidTransform ${EXTERNAL_LIBRARIES})
        
        add_executable (TestGainVariation TestGainVariation.cc)
        target_link_libraries (TestGainVariation Nymph KatydidUtility KatydidSpectrumAnalysis KatydidIO ${EXTERNAL_LIBRARIES})
    
        add_executable (TestROOTDictionary TestROOTDictionary.cc)
        target_link_libraries (TestROOTDictionary Nymph KatydidUtility KatydidData KatydidIO ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestBasicROOTFileWriter
                    TestGainVariation
                    TestROOTDictionary
        )
    endif (ROOT_FOUND)
    
    # executables that DO require Monarch
    if (Katydid_USE_MONARCH)
        add_executable (TestEggHatching TestEggHatching.cc)
        target_link_libraries (TestEggHatching Nymph KatydidUtility KatydidData KatydidIO KatydidTime ${EXTERNAL_LIBRARIES})
        
        pbuilder_install_executables ( 
                    TestEggHatching
        )
    endif (Katydid_USE_MONARCH)
        
endif (Katydid_ENABLE_TESTING) 
