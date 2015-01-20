# CMakeLists for Katydid/Source/Executables/Main
# Author: N. Oblath


# Executables

add_executable (EvaluateCandidates ${PROJECT_SOURCE_DIR}/KTCore/Executables/Main/Katydid.cc)
target_link_libraries (EvaluateCandidates KTCore KatydidUtility KatydidData KatydidIO KatydidEvaluation ${EXTERNAL_LIBRARIES})

add_executable (Truncate Truncate.cc)
target_link_libraries (Truncate KTCore ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    EvaluateCandidates
    Truncate
)

if (Katydid_USE_MONARCH)
    add_executable (EggScanner EggScanner.cc)
    target_link_libraries (EggScanner KTCore KatydidUtility KatydidData KatydidIO KatydidTime ${EXTERNAL_LIBRARIES})
    
    pbuilder_install_executables ( 
        EggScanner
    )
endif (Katydid_USE_MONARCH)

if (Katydid_USE_MATLAB)
    add_executable (RSAMatToEgg RSAMatToEgg.cc)
    target_link_libraries (RSAMatToEgg KTCore KatydidUtility KatydidData KatydidTime ${EXTERNAL_LIBRARIES})
    
    pbuilder_install_executables (
        RSAMatToEgg
    )
endif (Katydid_USE_MATLAB)