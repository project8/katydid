# CMakeLists for Katydid/Source/Time
# Author: N. Oblath

set (TIME_HEADERFILES
    KTDAC.hh
    KTDigitizerTests.hh
    KTEggProcessor.hh
    KTEggReader.hh
    KTEgg1Reader.hh
    KTSingleChannelDAC.hh
)

set (TIME_SOURCEFILES
    KTDAC.cc
    KTDigitizerTests.cc
    KTEggProcessor.cc
    KTEggReader.cc
    KTEgg1Reader.cc
    KTSingleChannelDAC.cc
)

if (Katydid_USE_MONARCH)

    if( Monarch_BUILD_MONARCH2 )
        set (TIME_HEADERFILES
            ${TIME_HEADERFILES}
            KTEgg2Reader.hh
        )
    endif( Monarch_BUILD_MONARCH2 )
    if( Monarch_BUILD_MONARCH3 )
        set (TIME_HEADERFILES
            ${TIME_HEADERFILES}
            KTEgg3Reader.hh
        )
    endif( Monarch_BUILD_MONARCH3 )
    #KTEggWriter.hh
    
    if( Monarch_BUILD_MONARCH2 )
        set (TIME_SOURCEFILES
            ${TIME_SOURCEFILES}
            KTEgg2Reader.cc
        )
    endif( Monarch_BUILD_MONARCH2 )
    if( Monarch_BUILD_MONARCH3 )
        set (TIME_SOURCEFILES
            ${TIME_SOURCEFILES}
            KTEgg3Reader.cc
        )
    endif( Monarch_BUILD_MONARCH3 )
    #KTEggWriter.cc
endif (Katydid_USE_MONARCH)

if (Katydid_USE_MATLAB)
    set (TIME_HEADERFILES
        ${TIME_HEADERFILES}
        KTRSAMatReader.hh
    )
    set (TIME_SOURCEFILES
        ${TIME_SOURCEFILES}
        KTRSAMatReader.cc
    )
endif (Katydid_USE_MATLAB)


set (KATYDID_LIBS
    KatydidUtility
    KatydidData
    KatydidIO
)

##################################################

add_library (KatydidTime ${TIME_SOURCEFILES})
target_link_libraries (KatydidTime ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES} ${EXTERNAL_LIBRARIES_MATLAB})

pbuilder_install_headers (${TIME_HEADERFILES})
pbuilder_install_libraries (KatydidTime)

#if (Katydid_USE_PYTHON)
#    add_library (katydidPy SHARED TimeLibPy.cc)
#    target_link_libraries (katydidPy KatydidTime ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES} ${EXTERNAL_LIBRARIES_MATLAB})
#    set_target_properties (katydidPy PROPERTIES PREFIX "")
#    pbuilder_install_headers (${TIME_PY_HEADERFILES})
#    pbuilder_install_libraries (katydidPy)
#endif (Katydid_USE_PYTHON)
