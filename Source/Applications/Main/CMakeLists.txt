# CMakeLists for Katydid/Source/Executables/Main
# Author: N. Oblath


# Executables

add_executable (EvaluateCandidates ${PROJECT_SOURCE_DIR}/Source/KTCore/Executables/Main/Katydid.cc)
target_link_libraries (EvaluateCandidates KTCore KatydidUtility KatydidData KatydidIO KatydidEvaluation ${EXTERNAL_LIBRARIES})

add_executable (Truncate Truncate.cc)
target_link_libraries (Truncate KTCore ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    EvaluateCandidates
    Truncate
)

if (Katydid_USE_MONARCH)
    add_executable (EggScanner EggScanner.cc)
    target_link_libraries (EggScanner KTCore KatydidUtility KatydidData KatydidIO KatydidEgg ${EXTERNAL_LIBRARIES})
    
    pbuilder_install_executables ( 
        EggScanner
    )
endif (Katydid_USE_MONARCH)
