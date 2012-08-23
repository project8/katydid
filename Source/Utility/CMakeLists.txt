# CMakeLists for Katydid/Source/Utility
# Author: N. Oblath

if (LOG4CXX_FOUND)
    set (LOGGER_CONFIGURATION "${CONFIG_INSTALL_DIR}/log4cxx.properties" )
    set_source_files_properties (KTLogger.cc PROPERTIES COMPILE_DEFINITIONS
        "LOG4CXX;LOGGER_CONFIGURATION=\"${LOGGER_CONFIGURATION}\""
    )
endif (LOG4CXX_FOUND)

set (UTILITY_DICT_HEADER_FILES)

set (UTILITY_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar_specialization.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar_class.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar_operators.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar_functions.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties_GetNBins.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTCutableArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDestroyer.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFactory.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTMaskedArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArray.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSingleton.hh
)

set (UTILITY_HEADERFILES ${UTILITY_DICT_HEADERFILES} ${UTILITY_NODICT_HEADERFILES})

set (UTILITY_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/complexpolar_specialization.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTAxisProperties.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTLogger.cc
)

#if (ROOT_FOUND)
#    set (UTILITY_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/UtilityLinkDef.hh)
#    set (UTILITY_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/UtilityDict.cxx)
#endif (ROOT_FOUND)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${UTILITY_DICT_HEADERFILES}" "${UTILITY_LINKDEF_HEADERFILE}" "${UTILITY_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidUtility ${UTILITY_SOURCEFILES})
set_target_properties(KatydidUtility PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
target_link_libraries (KatydidUtility ${LIBS})

install (FILES ${UTILITY_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
install (TARGETS KatydidUtility  DESTINATION ${LIB_INSTALL_DIR})

# install the log4cxx config file
install (FILES config/log4cxx.properties DESTINATION ${CONFIG_INSTALL_DIR})

