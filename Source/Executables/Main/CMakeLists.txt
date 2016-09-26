# CMakeLists for Katydid/Source/Executables/Main
# Author: N. Oblath

if( Katydid_ENABLE_EXECUTABLES )

    # Executables

    get_property( all_libs GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES )
    message(STATUS "Katydid exectuables library dependencies: ${all_libs}" )

    set( programs
        Katydid
        Truncate
    )
    
    if (Katydid_USE_MONARCH)
        set( programs ${programs}
            EggScanner
        )
    endif (Katydid_USE_MONARCH)
    
    if (Katydid_USE_MATLAB AND Monarch_BUILD_MONARCH3)
         set( programs ${programs}
            RSAMatToEgg
        )
    endif (Katydid_USE_MATLAB AND Monarch_BUILD_MONARCH3)

    pbuilder_executables( programs all_libs )

endif( Katydid_ENABLE_EXECUTABLES )
