# CMakeLists for Katydid/Source/KTCore/Executables/Main
# Author: N. Oblath

# Executables

add_executable (ConfigCheck ConfigCheck.cc)
target_link_libraries (ConfigCheck KTCore ${EXTERNAL_LIBRARIES})

add_executable (Katydid Katydid.cc)
target_link_libraries (Katydid KTCore KatydidUtility KatydidData KatydidIO KatydidSimulation KatydidEgg KatydidFFT KatydidAnalysis KatydidCandidates ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    ConfigCheck
    Katydid
)
