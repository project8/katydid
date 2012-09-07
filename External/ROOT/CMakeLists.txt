# CMakeLists for Katydid/External/ROOT
# Author: N. Oblath

if (NOT ROOT_FOUND)

    set (IMPORTED_HEADERFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/PdfFuncMathCore.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ProbFuncMathCore.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RConfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Riostream.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rtypeinfo.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rtypes.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RVersion.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SpecFuncMathCore.h
        ${CMAKE_CURRENT_SOURCE_DIR}/TError.h
        ${CMAKE_CURRENT_SOURCE_DIR}/TMath.h
        ${CMAKE_CURRENT_SOURCE_DIR}/TMathBase.h
    )
    
    install (FILES ${IMPORTED_HEADERFILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
    
endif(NOT ROOT_FOUND)
