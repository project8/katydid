# CMakeLists for Katydid/KTCore/Executables/Validation
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
    
    
    # Executables
    
    add_executable (TestApplication TestApplication.cc
                                    KTTestConfigurable.cc)
    target_link_libraries (TestApplication KTCore ${EXTERNAL_LIBRARIES})

    add_executable (TestApplyCut TestApplyCut.cc)
    target_link_libraries (TestApplyCut KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCacheDirectory TestCacheDirectory.cc)
    target_link_libraries (TestCacheDirectory KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestConfigurator TestConfigurator.cc
                                       KTTestConfigurable.cc)
    target_link_libraries (TestConfigurator KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestCut TestCut.cc)
    target_link_libraries (TestCut KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestLogger TestLogger.cc)
    target_link_libraries (TestLogger KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestPrintData TestPrintData.cc)
    target_link_libraries (TestPrintData KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestSignalsAndSlots TestSignalsAndSlots.cc
                                        KTTestProcessor.cc
    )
    target_link_libraries (TestSignalsAndSlots KTCore ${EXTERNAL_LIBRARIES})
    
    add_executable (TestThroughputProfiler TestThroughputProfiler.cc)
    target_link_libraries (TestThroughputProfiler KTCore ${EXTERNAL_LIBRARIES})

    pbuilder_install_executables (
                     TestApplication
                     TestApplyCut
                     TestCacheDirectory
                     TestConfigurator
                     TestCut
                     TestLogger
                     TestSignalsAndSlots
                     TestThroughputProfiler
    )
        
endif (Katydid_ENABLE_TESTING) 
