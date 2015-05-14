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
    add_definitions(-DUSE_MONARCH)

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

if (Katydid_USE_MANTIS)
    set (TIME_HEADERFILES
        ${TIME_HEADERFILES}
        KTMantis.hh
        KTMantisClientWriting.hh
        KTMantisWriterToTS.hh
    )
    set (TIME_SOURCEFILES
        ${TIME_SOURCEFILES}
        KTMantis.cc
        KTMantisClientWriting.cc
        KTMantisWriterToTS.cc
    )
endif (Katydid_USE_MANTIS)

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
    KTCore
    KatydidUtility
    KatydidData
    KatydidIO
)

##################################################

add_library (KatydidTime ${TIME_SOURCEFILES})
target_link_libraries (KatydidTime ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES} ${EXTERNAL_LIBRARIES_MATLAB})

pbuilder_install_headers (${TIME_HEADERFILES})
pbuilder_install_libraries (KatydidTime)
         
