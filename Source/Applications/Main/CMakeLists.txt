# CMakeLists for Katydid/Source/Applications/Main
# Author: N. Oblath

# Config files
set (VALIDATION_CONFIGFILES
    ConfigFiles/DisplayFFT.json
    ConfigFiles/EvaluateCandidates.json
    ConfigFiles/FSPeakAnalysisConfig.json
    ConfigFiles/KatydidPSAnalysisTemplate.json
    ConfigFiles/KatydidPSComplexConfig.json
    ConfigFiles/KatydidPSConfig.json
    ConfigFiles/KatydidWVConfig.json
    ConfigFiles/ts-plot-direct-from-mantis.json
)

pbuilder_install_config (${VALIDATION_CONFIGFILES})
    

# Executables

add_executable (EvaluateCandidates Katydid.cc)
target_link_libraries (EvaluateCandidates KatydidUtility KatydidCore KatydidData KatydidIO KatydidEvaluation ${EXTERNAL_LIBRARIES})

add_executable (ConfigCheck ConfigCheck.cc)
target_link_libraries (ConfigCheck KatydidUtility KatydidCore ${EXTERNAL_LIBRARIES})

add_executable (Katydid Katydid.cc)
target_link_libraries (Katydid KatydidUtility KatydidCore KatydidData KatydidIO KatydidSimulation KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

add_executable (Truncate Truncate.cc)
target_link_libraries (Truncate KatydidUtility ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    ConfigCheck
    EvaluateCandidates
    Katydid
    Truncate
)

if (Katydid_USE_MONARCH)
    add_executable (EggScanner EggScanner.cc)
    target_link_libraries (EggScanner KatydidUtility KatydidCore KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})
    
    pbuilder_install_executables ( 
        EggScanner
    )
endif (Katydid_USE_MONARCH)
