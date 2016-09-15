# CMakeLists for Katydid/Source/Applications/Profiling
# Author: N. Oblath

if (Katydid_ENABLE_PROFILING)

    # Config files
    set (PROFILING_CONFIGFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ImpulseResponseConfig.json
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ProfileSimpleFFTConfig.json
    )
    
    pbuilder_install_config (${PROFILING_CONFIGFILES})
    

    # Headers for any test classes
    #set (PROFILING_HEADERFILES
    #)
    
    #pbuilder_install_headers (${PROFILING_HEADERFILES})
    
    
    # Executables that do NOT require ROOT, Monarch or FFTW
    
    #set( LIB_DEPENDENCIES
    #    KatydidUtility
    #    KatydidData
    #    KatydidIO
    #    KatydidTime
    #    KatydidTransform
    #    KatydidSpectrumAnalysis
    #    KatydidSimulation
    #)
    
    #set( PROGRAMS
        #ImpulseResponse # disabled because it requires the Simulations library and KTSinusoidGenerator
        #ProfileDataAccessMethods # disabled because it requires the Simulations library and KTSinusoidGenerator
    #)
    
    #pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )
             
    
    if (Katydid_USE_MONARCH AND Monarch_BUILD_MONARCH2 AND FFTW_FOUND)
    
        set( LIB_DEPENDENCIES
            KatydidUtility
            KatydidData
            KatydidIO
            KatydidTime
            KatydidTransform
            KatydidSpectrumAnalysis
            KatydidSimulation
        )
        
        set( PROGRAMS
            ProfileFFTWandMonarch
            ProfileManualAnalysis
        )
        
        pbuilder_executables( PROGRAMS LIB_DEPENDENCIES )

    endif (Katydid_USE_MONARCH AND Monarch_BUILD_MONARCH2 AND FFTW_FOUND)

endif (Katydid_ENABLE_PROFILING) 
