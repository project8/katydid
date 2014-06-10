# CMakeLists for Katydid/Source/Egg
# Author: N. Oblath

set (EGG_DICT_HEADERFILES
)

set (EGG_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDAC.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDigitizerTests.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.hh
)

set (EGG_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDAC.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDigitizerTests.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.cc
)

if (Katydid_USE_MONARCH)
    set (EGG_NODICT_HEADERFILES
        ${EGG_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.hh
        #${CMAKE_CURRENT_SOURCE_DIR}/KTEggWriter.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.cc
        #${CMAKE_CURRENT_SOURCE_DIR}/KTEggWriter.cc
    )
endif (Katydid_USE_MONARCH)

if (Katydid_USE_MANTIS)
    set (EGG_NODICT_HEADERFILES
        ${EGG_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantis.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantisClientWriting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantisWriterToTS.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantis.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantisClientWriting.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMantisWriterToTS.cc
    )
endif (Katydid_USE_MANTIS)

if (Katydid_USE_MATLAB)
    set (EGG_NODICT_HEADERFILES
        ${EGG_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTRSAMatReader.hh
    )
    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTRSAMatReader.cc
    )
endif (Katydid_USE_MATLAB)


set (EGG_HEADERFILES ${EGG_DICT_HEADERFILES} ${EGG_NODICT_HEADERFILES})

#set (EGG_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EggLinkDef.hh)
#set (EGG_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EggDict.cxx)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
    KatydidIO
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${EGG_DICT_HEADERFILES}" "${EGG_LINKDEF_HEADERFILE}" "${EGG_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidEgg ${EGG_SOURCEFILES} ${EGG_DICT_OUTFILE})
target_link_libraries (KatydidEgg ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${EGG_HEADERFILES})
pbuilder_install_libraries (KatydidEgg)
         
