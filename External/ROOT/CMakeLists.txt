# CMakeLists for Katydid/External/ROOT
# Author: N. Oblath

if (NOT ROOT_FOUND)

    set (IMPORTED_HEADERFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/RConfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rtypeinfo.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rtypes.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RVersion.h
    )
    
    install (FILES ${IMPORTED_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
    
endif(NOT ROOT_FOUND)
