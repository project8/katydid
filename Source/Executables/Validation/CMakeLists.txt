# CMakeLists for Katydid/Source/Executables/Validation
# Author: N. Oblath

if (Katydid_ENABLE_TESTING)

    # Executables that do NOT require ROOT, FFTW, or Monarch
    
    # Utility tests

    set( LIB_DEPENDENCIES
        KatydidUtility 
    )
    
    set( PROGRAMS
        ObjectSize
        TestAxisProperties
        TestComplexPolar
        TestCutableArray
        TestCutIterator
        TestMinMaxBin
        TestRandom
        TestVector
    )
    
    pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )


    # Data & IO tests

    set( LIB_DEPENDENCIES
        KatydidUtility
        KatydidData
        KatydidIO
    )
    
    set( PROGRAMS
        TestDataDisplay
        TestDPTReader
        #TestFrequencySpectrumFFTW
        #TestJSONWriter
        #TestMultiFileJSONReader
        TestSmoothing
        #TestASCIIFileWriter
    )
    
    pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )


    # Time, Transform, SpectrumAnalysis & EventAnalysis

    set( LIB_DEPENDENCIES
        KatydidUtility
        KatydidData
        KatydidIO
        KatydidTime
        KatydidTransform
        KatydidSpectrumAnalysis
        KatydidEventAnalysis
    )
    
    set( PROGRAMS
        Test2DDiscrim
        TestConsensusThresholding
        TestCorrelator
        TestDataAccumulator
        TestDBScan
        TestDBScanTrackClustering
        TestDistanceClustering
        #TestHoughTransform  # temporarily disabled because it's not compatible with the changes made while introducing the extensible data scheme
        TestKDTree
        TestKDTreeData
        TestLinearDensityProbe
        TestMultiSliceClustering
        TestNanoflann
        #TestSimpleClustering # disabled because it's written for the old version of KTMultiSliceClustering; see TestMultiSliceClustering
        #TestSlidingWindowFFT
        TestSpectrogramCollector
        TestSpectrumDiscriminator
        TestWindowFunction
    )
    
    pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )

             
    # executables that DO require FFTW
    
    if (FFTW_FOUND)
    
        set( LIB_DEPENDENCIES
            KatydidUtility
            KatydidData
            KatydidIO
            KatydidTransform
            KatydidSpectrumAnalysis
        )
        
        set( PROGRAMS
           TestForwardFFTW
           TestWignerVille
        )
        
        pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )

    endif (FFTW_FOUND)
    
    
    # executables that DO require ROOT
    
    if (ROOT_FOUND)

        set( LIB_DEPENDENCIES
            KatydidUtility
            KatydidData
            KatydidIO
            KatydidTransform
            KatydidSpectrumAnalysis
        )
        
        set( PROGRAMS
           TestBasicROOTFileWriter
           TestGainVariation
           TestROOTDictionary
           TestROOTWriterFileManager
           TestROOTWritingToSameFile
        )
        
        pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )

    endif (ROOT_FOUND)
    
    # executables that DO require Monarch
    
    if (Katydid_USE_MONARCH)
    
        set( LIB_DEPENDENCIES
            KatydidUtility
            KatydidData
            KatydidIO
            KatydidTime
        )
        
        set( PROGRAMS
           TestEggHatching
        )
        
        pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )

    endif (Katydid_USE_MONARCH)
        
endif (Katydid_ENABLE_TESTING) 
