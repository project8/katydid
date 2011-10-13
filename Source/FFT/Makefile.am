# $Id$
# Katydid/Source/FFT Makefile definition
# Author: Noah Oblath

FFT_INCLUDES =  \
	-I@top_srcdir@/Source/Egg \
    -I@top_srcdir@/Source/FFT

FFT_DICT_INCLUDES=\
	$(ROOT_CXXFLAGS)\
    -I@top_srcdir@/Source/FFT

# headers for classes that should go into the root dictionary
FFT_DICT_HEADERFILES = \
    KTComplexVector.hh\
    KTEventWindowFunction.hh\
    KTFFT.hh\
    KTHammingWindow.hh\
    KTHannWindow.hh\
    KTPowerSpectrum.hh\
    KTRectangularWindow.hh\
    KTSimpleFFT.hh\
    KTSlidingWindowFFT.hh\
    KTWindowFunction.hh

# headers for classes that should not go into the root dictionary (e.g. if a class uses boost)
FFT_NODICT_HEADERFILES = 

FFT_HEADERFILES = \
    $(FFT_DICT_HEADERFILES)\
    $(FFT_NODICT_HEADERFILES)

FFT_SOURCEFILES = \
    KTComplexVector.cc\
    KTEventWindowFunction.cc\
    KTFFT.cc\
    KTHammingWindow.cc\
    KTHannWindow.cc\
    KTPowerSpectrum.cc\
    KTRectangularWindow.cc\
    KTSimpleFFT.cc\
    KTSlidingWindowFFT.cc\
    KTWindowFunction.cc


FFT_LINKDEF_HEADERFILES = \
    LinkDef/FFTLinkDef.hh

include_HEADERS = \
    $(FFT_HEADERFILES)

noinst_HEADERS = $(FFT_LINKDEF_HEADERFILES)

###########################################

noinst_LTLIBRARIES = libFFT.la

libFFT_la_CPPFLAGS = $(FFT_INCLUDES) $(AM_CPPFLAGS)

libFFT_la_SOURCES = \
    $(FFT_SOURCEFILES)

nodist_libFFT_la_SOURCES = \
    FFTDict.cxx

FFTDict.cxx : $(FFT_DICT_HEADERFILES)   $(FFT_LINKDEF_HEADERFILES)
	$(ROOTCINT) -f $@ -c $(AM_CXXFLAGS) $(FFT_DICT_INCLUDES) $+

###########################################

CLEANFILES = \
   FFTDict.cxx\
   FFTDict.h

MAINTAINERCLEANFILES = Makefile.in
