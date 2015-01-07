# CMakeLists for Katydid/Source/Egg
# Author: N. Oblath

set (EGG_HEADERFILES
    KTDAC.hh
    KTDigitizerTests.hh
    KTEggProcessor.hh
    KTEggReader.hh
    KTEgg1Reader.hh
)

set (EGG_SOURCEFILES
    KTDAC.cc
    KTDigitizerTests.cc
    KTEggProcessor.cc
    KTEggReader.cc
    KTEgg1Reader.cc
)

if (Katydid_USE_MONARCH)
    set (EGG_HEADERFILES
        ${EGG_HEADERFILES}
        KTEgg2Reader.hh
        KTEgg3Reader.hh
        #KTEggWriter.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        KTEgg2Reader.cc
        KTEgg3Reader.cc
        #KTEggWriter.cc
    )
endif (Katydid_USE_MONARCH)

if (Katydid_USE_MANTIS)
    set (EGG_HEADERFILES
        ${EGG_HEADERFILES}
        KTMantis.hh
        KTMantisClientWriting.hh
        KTMantisWriterToTS.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        KTMantis.cc
        KTMantisClientWriting.cc
        KTMantisWriterToTS.cc
    )
endif (Katydid_USE_MANTIS)

if (Katydid_USE_MATLAB)
    set (EGG_HEADERFILES
        ${EGG_HEADERFILES}
        KTRSAMatReader.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
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

add_library (KatydidTime ${EGG_SOURCEFILES})
target_link_libraries (KatydidTime ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES} ${EXTERNAL_LIBRARIES_MATLAB})

pbuilder_install_headers (${EGG_HEADERFILES})
pbuilder_install_libraries (KatydidTime)
         
