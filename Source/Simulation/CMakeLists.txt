# CMakeLists for Katydid/Source/Simulation
# Author: N. Oblath

set (SIMULATION_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDCOffsetGenerator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGaussianNoiseGenerator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSinusoidGenerator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTSGenerator.hh
)

set (SIMULATION_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDCOffsetGenerator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTGaussianNoiseGenerator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSinusoidGenerator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTSGenerator.cc
)

#if (ROOT_FOUND)
#    set (SIMULATION_HEADERFILES 
#        ${SIMULATION_HEADERFILES}   
#        ${CMAKE_CURRENT_SOURCE_DIR}/KT.hh
#    )
#    set (SIMULATION_SOURCEFILES
#        ${SIMULATION_SOURCEFILES}
#        ${CMAKE_CURRENT_SOURCE_DIR}/KT.cc
#    )
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    Nymph
    KatydidUtility
    KatydidData
)

##################################################

add_library (KatydidSimulation ${SIMULATION_SOURCEFILES})
target_link_libraries (KatydidSimulation ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_libraries(KatydidSimulation)
pbuilder_install_headers(${SIMULATION_HEADERFILES})
