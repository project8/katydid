# CMakeLists for Katydid/Source/FFT
# Author: N. Oblath

set (FFT_DICT_HEADERFILES
)

set (FFT_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTComplexFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEventWindowFunction.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFT.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHammingWindow.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHannWindow.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrum.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTRectangularWindow.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleFFT.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFFT.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFSData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFSDataFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWindowFunction.hh
)

if (ROOT_FOUND)
    set (FFT_NODICT_HEADERFILES
        ${FFT_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrumAverager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiEventROOTTypeWriterFFT.hh
    )
endif (ROOT_FOUND)

set (FFT_HEADERFILES ${FFT_DICT_HEADERFILES} ${FFT_NODICT_HEADERFILES})

set (FFT_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTComplexFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEventWindowFunction.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFFT.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumDataFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTFrequencySpectrumFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHammingWindow.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTHannWindow.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrum.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTRectangularWindow.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSimpleFFT.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFFT.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFSData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSlidingWindowFSDataFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTWindowFunction.cc
)

if (ROOT_FOUND)
    set (FFT_SOURCEFILES
        ${FFT_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTPowerSpectrumAverager.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterFFT.cc	
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiEventROOTTypeWriterFFT.cc
    )
endif (ROOT_FOUND)

#if (ROOT_FOUND)
#    set (FFT_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/FFTLinkDef.hh)
#    set (FFT_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/FFTDict.cxx)
#endif (ROOT_FOUND)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidOutput
    KatydidEgg
)

##################################################

#if (ROOT_FOUND)
#    ROOT_GENERATE_DICTIONARY ("${FFT_DICT_HEADERFILES}" "${FFT_LINKDEF_HEADERFILE}" "${FFT_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")
#endif (ROOT_FOUND)

add_library (KatydidFFT ${FFT_SOURCEFILES} ${FFT_DICT_OUTFILE})
target_link_libraries (KatydidFFT ${KATYDID_LIBS} ${LIBS})

katydid_install_headers (${FFT_HEADERFILES})
katydid_install_libraries (KatydidFFT)
         
