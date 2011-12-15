# CMakeLists for Katydid/Source/Egg
# Author: N. Oblath

set (EGG_DICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEgg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEvent.hh
)

set (EGG_NODICT_HEADERFILES)

set (EGG_HEADERFILES ${EGG_DICT_HEADERFILES} ${EGG_NODICT_HEADERFILES})

set (EGG_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEgg.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEvent.cc
)

set (EGG_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EggLinkDef.hh)
set (EGG_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EggDict.cxx)

##################################################

ROOT_GENERATE_DICTIONARY ("${EGG_DICT_HEADERFILES}" "${EGG_LINKDEF_HEADERFILE}" "${EGG_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidEgg ${EGG_SOURCEFILES} ${EGG_DICT_OUTFILE})
set_target_properties(KatydidEgg PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries (KatydidEgg KatydidUtility ${LIBS})

install (FILES ${EGG_HEADERFILES}  DESTINATION include)
install (TARGETS KatydidEgg  DESTINATION lib)
         
