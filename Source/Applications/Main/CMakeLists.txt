# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

# Config files
set (VALIDATION_CONFIGFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/EvaluateCandidates.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/FSPeakAnalysisConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSComplexConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidPSConfig.json
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles/KatydidWVConfig.json
)

pbuilder_install_config (${VALIDATION_CONFIGFILES})
    

# Executables

add_executable (EvaluateCandidates ${CMAKE_CURRENT_SOURCE_DIR}/Katydid.cc)
target_link_libraries (EvaluateCandidates KatydidUtility KatydidCore KatydidData KatydidIO KatydidEvaluation ${EXTERNAL_LIBRARIES})

add_executable (ConfigCheck ${CMAKE_CURRENT_SOURCE_DIR}/ConfigCheck.cc)
target_link_libraries (ConfigCheck KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})

add_executable (Katydid ${CMAKE_CURRENT_SOURCE_DIR}/Katydid.cc)
target_link_libraries (Katydid KatydidUtility KatydidCore KatydidData KatydidIO KatydidSimulation KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

add_executable (Truncate ${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cc)
target_link_libraries (Truncate KatydidUtility ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    ConfigCheck
    EvaluateCandidates
    Katydid
    Truncate
)

if (Katydid_USE_MONARCH)
    add_executable (EggScanner ${CMAKE_CURRENT_SOURCE_DIR}/EggScanner.cc)
    target_link_libraries (EggScanner KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})
    
    pbuilder_install_executables ( 
        EggScanner
    )
endif (Katydid_USE_MONARCH)
