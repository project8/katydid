# CMakeLists for Katydid/Source/Framework
# Author: N. Oblath

set (FRAMEWORK_DICT_HEADERFILES)

set (FRAMEWORK_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManager.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManagerFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManagerFactoryPrototype.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManagerTable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrameworkMessage.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManager.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManagerFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManagerFactoryPrototype.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManagerTable.hh
)

set (FRAMEWORK_HEADERFILES ${FRAMEWORK_DICT_HEADERFILES} ${FRAMEWORK_NODICT_HEADERFILES})

set (FRAMEWORK_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManager.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManagerFactory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCoreManagerTable.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrameworkMessage.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManager.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManagerFactory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTManagerTable.cc
)

#set (FRAMEWORK_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/Linkdef/FrameworkLinkDef.hh)
#set (FRAMEWORK_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/FrameworkDict.cxx)

##################################################

#ROOT_GENERATE_DICTIONARY ("${FRAMEWORK_DICT_HEADERFILES}" "${FRAMEWORK_LINKDEF_HEADERFILE}" "${FRAMEWORK_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidFramework ${FRAMEWORK_SOURCEFILES} ${FRAMEWORK_DICT_OUTFILE})
set_target_properties(KatydidFramework PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries (KatydidFramework KatydidUtility ${LIBS})

install (FILES ${FRAMEWORK_HEADERFILES}  DESTINATION include)
install (TARGETS KatydidFramework  DESTINATION lib)
         