# CMakeLists for Katydid/Source/Applications/Profiling
# Author: N. Oblath

if (Katydid_ENABLE_PROFILING)

    # Config files
    set (PROFILING_CONFIGFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/ProfileSimpleFFTConfig.json
    )
    
    katydid_install_config (${PROFILING_CONFIGFILES})
    

    # Headers for any test classes
    #set (PROFILING_HEADERFILES
    #)
    
    #katydid_install_headers (${PROFILING_HEADERFILES})
    
    
    # Executables that do NOT require ROOT
    add_executable (ProfileDataAccessMethods ${CMAKE_CURRENT_SOURCE_DIR}/ProfileDataAccessMethods.cc)
    target_link_libraries (ProfileDataAccessMethods KatydidUtility KatydidCore KatydidData KatydidEgg ${LIBS})
        
    add_executable (ProfileFFTWandMonarch ${CMAKE_CURRENT_SOURCE_DIR}/ProfileFFTWandMonarch.cc)
    target_link_libraries (ProfileFFTWandMonarch KatydidUtility KatydidCore KatydidData KatydidEgg KatydidFFT ${LIBS})

    add_executable (ProfileManualAnalysis ${CMAKE_CURRENT_SOURCE_DIR}/ProfileManualAnalysis.cc)
    target_link_libraries (ProfileManualAnalysis KatydidUtility KatydidCore KatydidData KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${LIBS})

    katydid_install_executables (
        ProfileDataAccessMethods
        ProfileFFTWandMonarch
        ProfileManualAnalysis
    )
             
    # executables that DO require ROOT
    if (ROOT_FOUND)
        #add_executable (TestBasicROOTFileWriter ${CMAKE_CURRENT_SOURCE_DIR}/TestBasicROOTFileWriter.cc)
        #target_link_libraries (TestBasicROOTFileWriter KatydidUtility KatydidCore KatydidData KatydidEgg KatydidFFT KatydidOutput ${LIBS})
        
        #katydid_install_executables ( 
        #)
    
    endif (ROOT_FOUND)
        
endif (Katydid_ENABLE_PROFILING) 
