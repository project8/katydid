# CMakeLists for Katydid/Source/Simulation
# Author: N. Oblath

set (SIMULATION_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTSGenerator.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSinusoidGenerator.hh
)

set (SIMULATION_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTSGenerator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSinusoidGenerator.cc
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
    KatydidUtility
    KatydidCore
    KatydidData
)

##################################################

add_library (KatydidSimulation ${SIMULATION_SOURCEFILES})
target_link_libraries (KatydidSimulation ${KATYDID_LIBS} ${LIBS})

pbuilder_install_libraries(KatydidSimulation)
pbuilder_install_headers(${SIMULATION_HEADERFILES})
