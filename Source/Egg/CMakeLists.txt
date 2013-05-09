# CMakeLists for Katydid/Source/Egg
# Author: N. Oblath

set (EGG_DICT_HEADERFILES
)

set (EGG_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/MonarchPP.hh
)

set (EGG_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/MonarchPP.cc
)

set (EGG_HEADERFILES ${EGG_DICT_HEADERFILES} ${EGG_NODICT_HEADERFILES})

#set (EGG_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EggLinkDef.hh)
#set (EGG_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EggDict.cxx)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidData
    KatydidOutput
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${EGG_DICT_HEADERFILES}" "${EGG_LINKDEF_HEADERFILE}" "${EGG_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidEgg ${EGG_SOURCEFILES} ${EGG_DICT_OUTFILE})
target_link_libraries (KatydidEgg ${KATYDID_LIBS} ${LIBS})

pbuilder_install_headers (${EGG_HEADERFILES})
pbuilder_install_libraries (KatydidEgg)
         
