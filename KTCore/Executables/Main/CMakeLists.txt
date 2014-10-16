# CMakeLists for Katydid/KTCore/Executables/Main
# Author: N. Oblath

# Executables

add_executable (ConfigCheck ConfigCheck.cc)
target_link_libraries (ConfigCheck KTCore ${EXTERNAL_LIBRARIES})

add_executable (Katydid Katydid.cc)
get_property(ALL_LIBS GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES)
message(STATUS "Linking these libraries to Katydid (plus external libs): ${ALL_LIBS}")
target_link_libraries (Katydid ${ALL_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_executables ( 
    ConfigCheck
    Katydid
)
