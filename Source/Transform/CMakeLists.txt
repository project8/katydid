# CMakeLists for Katydid/Source/Transform
# Author: N. Oblath

set (TRANSFORM_DICT_HEADERFILES
)

set (TRANSFORM_NODICT_HEADERFILES
    KTBlackmanHarrisWindow.hh
    KTConvertToPower.hh
    KTFFT.hh
    KTHammingWindow.hh
    KTHannWindow.hh
    KTRectangularWindow.hh
    KTSincWindow.hh
    KTWindower.hh
    KTWindowFunction.hh
)

if (FFTW_FOUND)
    set (TRANSFORM_NODICT_HEADERFILES
        ${TRANSFORM_NODICT_HEADERFILES}
        KTForwardFFTW.hh
        KTFractionalFFT.hh
        KTReverseFFTW.hh
    )
endif (FFTW_FOUND)        

set (TRANSFORM_HEADERFILES ${TRANSFORM_DICT_HEADERFILES} ${TRANSFORM_NODICT_HEADERFILES})

set (TRANSFORM_SOURCEFILES
    KTBlackmanHarrisWindow.cc
    KTConvertToPower.cc
    KTFFT.cc
    KTHammingWindow.cc
    KTHannWindow.cc
    KTRectangularWindow.cc
    KTSincWindow.cc
    KTWindower.cc
    KTWindowFunction.cc
)

if (FFTW_FOUND)
    set (TRANSFORM_SOURCEFILES
        ${TRANSFORM_SOURCEFILES}
        KTForwardFFTW.cc
        KTFractionalFFT.cc
        KTReverseFFTW.cc
    )
endif (FFTW_FOUND)        

#if (ROOT_FOUND)
#    set (TRANSFORM_LINKDEF_HEADERFILE LinkDef/TransformLinkDef.hh)
#    set (TRANSFORM_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/TransformDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidData
    KatydidIO
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${TRANSFORM_DICT_HEADERFILES}" "${TRANSFORM_LINKDEF_HEADERFILE}" "${TRANSFORM_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidTransform ${TRANSFORM_SOURCEFILES} ${TRANSFORM_DICT_OUTFILE})
target_link_libraries (KatydidTransform ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${TRANSFORM_HEADERFILES})
pbuilder_install_libraries (KatydidTransform)
         
