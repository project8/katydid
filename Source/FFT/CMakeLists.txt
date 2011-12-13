add_library(KatydidFFT
    ${PROJECT_CURRENT_SOURCE_DIR}/KTComplexVector.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTEventWindowFunction.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTFFT.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTHammingWindow.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTHannWindow.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTPowerSpectrum.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTRectangularWindow.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTSimpleFFT.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTSlidingWindowFFT.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTWindowFunction.cc
)

install (FILES ${PROJECT_CURRENT_SOURCE_DIR}/KTComplexVector.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTEventWindowFunction.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTFFT.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTHammingWindow.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTHannWindow.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTPowerSpectrum.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTRectangularWindow.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTSimpleFFT.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTSlidingWindowFFT.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTWindowFunction.hh
         DESTINATION include)
         