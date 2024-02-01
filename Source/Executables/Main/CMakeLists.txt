# CMakeLists for Katydid/Source/Executables/Main
# Author: N. Oblath

if( Katydid_ENABLE_EXECUTABLES )

    set( Katydid_LIBS
    
    )

    set( Katydid_SOURCES
        Katydid.cc
#        Truncate.cc
    )

#    if (Katydid_USE_MONARCH)
#        list( APPEND Katydid_SOURCES
#            EggScanner.cc
#        )
#    endif (Katydid_USE_MONARCH)
    
#    if (Katydid_USE_MATLAB AND Monarch_BUILD_MONARCH3)
#        list( APPEND Katydid_SOURCES
#            RSAMatToEgg.cc
#        )
#    endif (Katydid_USE_MATLAB AND Monarch_BUILD_MONARCH3)

    set( Katydid_PROGRAMS )
    pbuilder_executables( 
        SOURCES ${Katydid_SOURCES}
        TARGETS_VAR Katydid_PROGRAMS
        PROJECT_LIBRARIES ${Katydid_LIBS}
    #    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXE_LIBRARIES}
    #    PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXE_LIBS}
    )

    pbuilder_component_install_and_export( 
        COMPONENT Executables
        EXETARGETS ${Katydid_PROGRAMS}
    )

endif( Katydid_ENABLE_EXECUTABLES )
