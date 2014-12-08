# CMakeLists for Katydid/Source/FFT
# Author: N. Oblath

set (FFT_DICT_HEADERFILES
)

set (FFT_NODICT_HEADERFILES
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
    set (FFT_NODICT_HEADERFILES
        ${FFT_NODICT_HEADERFILES}
        KTForwardFFTW.hh
        #KTReverseFFTW.hh
    )
endif (FFTW_FOUND)        

set (FFT_HEADERFILES ${FFT_DICT_HEADERFILES} ${FFT_NODICT_HEADERFILES})

set (FFT_SOURCEFILES
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
    set (FFT_SOURCEFILES
        ${FFT_SOURCEFILES}
        KTForwardFFTW.cc
        #KTReverseFFTW.cc
    )
endif (FFTW_FOUND)        

#if (ROOT_FOUND)
#    set (FFT_LINKDEF_HEADERFILE LinkDef/FFTLinkDef.hh)
#    set (FFT_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/FFTDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KTCore
    KatydidUtility
    KatydidData
    KatydidIO
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${FFT_DICT_HEADERFILES}" "${FFT_LINKDEF_HEADERFILE}" "${FFT_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidFFT ${FFT_SOURCEFILES} ${FFT_DICT_OUTFILE})
target_link_libraries (KatydidFFT ${KATYDID_LIBS} ${EXTERNAL_LIBRARIES})

pbuilder_install_headers (${FFT_HEADERFILES})
pbuilder_install_libraries (KatydidFFT)
         
